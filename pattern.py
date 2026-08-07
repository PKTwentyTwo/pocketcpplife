'''Main Python bindings.'''
from ctypes import *
import os
try:
    from .compiler import compilelibrary, libdir, rootdir
    from .config import canoniserule
except ImportError:
    from compiler import compilelibrary, libdir, rootdir
    from config import canoniserule
def find_lib(rule):
    '''Returns the library needed for a given rule.'''
    rule = canoniserule(rule)
    target = libdir + '/' + rule + '.so'
    if os.path.isfile(target):
        return target
    compilelibrary(rule)
    return target
restype_dict = {'void*': c_void_p, 'uint64_t': c_uint64, 'uint32_t': c_uint32, 'int64_t': c_int64, 'int32_t': c_int32, 'bool': c_bool, 'char*': c_char_p, 'void': None}
def load_restypes():
    '''Loads the restypes for each function.'''
    file = rootdir + '/main.cpp'
    with open(file, 'r', encoding='utf-8') as f:
        data = f.readlines()
    rdict = {}
    for x in data:
        if len(x) < 5:
            continue
        if x[0] in ('e', '#', '/', '}', ' ', '\t'):
            continue
        stringres, funcname = x[:x.index('(')].split(' ')
        rdict[funcname] = restype_dict[stringres]
    return rdict
restypes = load_restypes()
#Dictionary of libraries:
libraries = {}
class WrappedLib:
    '''Wrapper class around a shared object.'''
    def __init__(self, file):
        if os.path.isfile(file):
            file = os.path.realpath(file)
        self.lib = CDLL(file)
    def make_compatible(self, data):
        '''Ensures the given data is compatible with the API.'''
        if isinstance(data, list):
            if len(data) == 1:
                #String buffer for copying RLEs.
                return create_string_buffer(data[0])
            if len(data) in (2, 4):
                #Buffer for copying int32_t pairs.
                return (c_int32 * len(data))(*data)
        if isinstance(data, str):
            #Encode to UTF-8:
            return data.encode('utf-8')
        return data
    def to_str(self, charp):
        '''Converts a character pointer to a Python string.'''
        data = charp.raw
        while not data[-1]:
            data = data[:-1]
        return data.decode('utf-8')
    def __call__(self, *args):
        '''Calls the given function.'''
        if len(args) < 2:
            raise TypeError('WrappedLib requires at least 2 arguments.')
        fname = args[0]
        try:
            func = getattr(self.lib, fname)
        except AttributeError:
            raise AttributeError('Unable to locate symbol with name "'+fname+'".')
        if fname in restypes:
            func.restype = restypes[fname]
        #Make sure we can return a string buffer:
        retpos = -1
        retbuftype = None
        for x in range(len(args)):
            val = args[x]
            if isinstance(val, list):
                retpos = x - 1
                if len(val) == 1:
                    retbuftype = str
                else:
                    retbuftype = int
        #Prepare arguments and call the function.
        fargs = [self.make_compatible(x) for x in args[1:]]
        ret = func(*fargs)
        if retpos == -1:
            return ret
        ret = fargs[retpos]
        if retbuftype == str:
            ret = self.to_str(ret)
            if not ret.startswith('!'):
                return ret
            length = int(ret[1:])
            return self(fname, [length], length)
        return ret
class PtStruct:
    '''Wrapper class used to avoid extra calls.'''
    def __init__(self, rule, ptr):
        self.rule = rule
        self.ptr = ptr
class Pattern:
    '''Main Pattern class.'''
    def __init__(self, rle = '', rule = 'b3s23'):
        self.rule = canoniserule(rule)
        if self.rule not in libraries:
            libraries[self.rule] = WrappedLib(find_lib(self.rule))
        self.lib = libraries[self.rule]
        #Actually initialise the pattern:
        if isinstance(rle, str):
            self.ptr = self.lib('NewPattern', rle)
        elif isinstance(rle, Pattern):
            self.ptr = self.lib('CopyPattern', rle)
            self.rule = rle.rule
            self.lib = libraries[self.rule]
        elif isinstance(rle, PtStruct):
            self.ptr = rle.ptr
            self.rule = rle.rule
            self.lib = libraries[self.rule]
        else:
            raise TypeError('Unable to initialise Pattern with argument of type '+str(type(rle))[7:-1])
    def advance(self, gens):
        '''Advances a pattern the specified number of generations.'''
        newptr = self.lib('AdvancePattern', self.ptr, gens)
        return Pattern(PtStruct(self.rule, newptr))
    def __getitem__(self, other):
        return self.advance(other)
    def rle_string(self) -> str:
        '''Gets the RLE string of a pattern.'''
        return self.lib('GetPatternRLE', self.ptr, [2048], 2048)
    def empty(self) -> bool:
        '''Returns True if the pattern is empty.'''
        return bool(self.lib('IsEmpty', self.ptr))
    def nonempty(self) -> bool:
        '''Returns True if the pattern is non-empty.'''
        return bool(self.lib('IsNonEmpty', self.ptr))
    def __bool__(self):
        return self.nonempty()
    def translate(self, dx, dy):
        '''Translates the pattern by (dx, dy).'''
        newptr = self.lib('TranslatePattern', self.ptr, dx, dy)
        return Pattern(PtStruct(self.rule, newptr))
    def shift(self, dx, dy):
        return self.translate(dx, dy)
    def transform(self, transformation):
        '''Transforms the pattern by the given transformation.'''
        transformations = ('identity', 'rot90', 'rot180', 'rot270', 'flip_x', 'flip_y', 'swap_xy', 'swap_xy_flip')
        if transformation not in transformations:
            raise ValueError('Transformation must be one of the following: '+str(transformations))
        newptr = self.lib('TransformPattern', self.ptr, transformation)
        return Pattern(PtStruct(self.rule, newptr))
    def __call__(self, *args):
        if len(args) == 1:
            return self.transform(args[0])
        if len(args) == 2:
            return self.transform(*args)
        raise TypeError("Usage: pt(dx, dy) or pt('rot90')")
    def digest(self) -> int:
        return self.lib('GetDigest', self.ptr)
    def __hash__(self):
        return self.digest()
    def __eq__(self, other):
        if not isinstance(other, Pattern):
            return False
        return self.lib('IsEqual', self.ptr, other.ptr)
    def __ne__(self, other):
        return not self == other
    def __add__(self, other):
        if not isinstance(other, Pattern):
            raise TypeError('Can only add other instances of Pattern to Pattern.')
        newptr = self.lib('AddPattern', self.ptr, other.ptr)
        return Pattern(PtStruct(self.rule, newptr))
    def __sub__(self, other):
        if not isinstance(other, Pattern):
            raise TypeError('Can only subtract other instances of Pattern from Pattern.')
        newptr = self.lib('SubtractPattern', self.ptr, other.ptr)
        return Pattern(PtStruct(self.rule, newptr))
    def getrect(self) -> list:
        '''Gets the bounding box of the pattern in the form [x, y, dx, dy].'''
        if self.empty():
            return None
        rect = self.lib('GetPatternRect', self.ptr, [0, 0, 0, 0])
        print(rect)
        return [rect[x] for x in range(4)]
    @property
    def bbox(self) -> list:
        '''Gets the bounding box of the pattern in the form [x, y, dx, dy].'''
        return self.getrect()
    @property
    def population(self) -> int:
        '''Gets the population of a pattern.'''
        return self.lib('GetPopulation', self.ptr)
    @property
    def period(self) -> int:
        '''Gets the period of a pattern.
Will throw an error if aperiodic.'''
        period = self.lib('GetPeriod', self.ptr)
        if period == 0:
            raise ValueError('Pattern is aperiodic.')
        return period
    @property
    def apgcode(self) -> str:
        '''Gets the apgcode of a pattern - 'aperiodic' if aperiodic.'''
        return self.lib('GetPatternApgcode', self.ptr, [2048], 2048)
    @property
    def displacement(self) -> tuple:
        '''Gets the displacement of a pattern - (0, 0) if aperiodic.'''
        disp = self.lib('GetDisplacement', self.ptr, [0, 0])
        return (disp[0], disp[1])
    def __del__(self):
        #Delete the C++ object:
        self.lib('DeletePattern', self.ptr)
    def __repr__(self):
        typename = str(type(self))[8:-2]      
        data = '(population = '+str(self.population)+', rule = '+self.rule+', pointer = '+str(self.ptr) + ')'
        return '<' + typename + data + '>'
