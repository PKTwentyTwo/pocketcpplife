'''This module contains a class, RuleHandler, for dealing with Hensel notation.'''
from re import match
import os
import sys
rootdir = os.path.dirname(__file__)
conditiondict = {
        '0':[''],
        '1':['c', 'e'],
        '2':['a','c','e','i','k','n'],
        '3':['a','c','e','i','j','k','n','q','r','y'],
        '4':['a','c','e','i','j','k','n','q','r','t','w','y','z'],
        '5':['a','c','e','i','j','k','n','q','r','y'],
        '6':['a','c','e','i','k','n'],
        '7':['c', 'e'],
        '8':['']}
def isvalid(rule):
    '''Checks if a rule is valid using regex.'''
    rule = rule.lower().replace('/', '')
    return bool(match('b[1-8ceaiknjqrytwz-]*s[0-8ceaiknjqrytwz-]*', rule))
def parserule(rule):
    '''Takes a rule and returns a list of birth and survival conditions.'''
    #I hate having to code parsers.
    if not isvalid(rule):
        raise ValueError('Rule does not match regex b[1-8ceaiknjqrytwz-]*s[0-8ceaiknjqrytwz-]*')
    #Convert the rule to a fixed format:
    rule = rule.lower().replace('/', '')
    conditions = []
    digits = ['0', '1', '2', '3', '4', '5', '6', '7', '8']
    cstring = ''
    birth = True
    rule = rule + '|' #Using a vertical bar to signify the end of the rulestring.
    for character in rule:
        if character in digits or character in ['b', 's', '|']:
            cstring = cstring.replace('b', '').replace('s', '')
            if cstring != '':
                digit = cstring[0]
                if len(cstring) == 1:
                    for x in conditiondict[cstring]:
                        if birth:
                            conditions.append('B' + cstring + x)
                        else:
                            conditions.append('S' + cstring + x)
                else:
                    subconditions = cstring[1:]
                    if subconditions[0] == '-':
                        temp = conditiondict[digit][:]
                        for x in cstring[2:]:
                            if x in temp:
                                temp.remove(x)
                        for x in temp:
                            if birth:
                                conditions.append('B' + digit + x)
                            else:
                                conditions.append('S' + digit + x)
                    else:
                        temp = []
                        for x in subconditions:
                            if x in conditiondict[digit]:
                                temp.append(x)
                        for x in temp:
                            if birth:
                                conditions.append('B' + digit + x)
                            else:
                                conditions.append('S' + digit + x)
            cstring = ''
        if character == 's':
            birth = False
        cstring = cstring + character
    for x in conditions[:]:
        if conditions.count(x) > 1:
            conditions.remove(x)
    return conditions
def tohensel(conditions):
    '''Takes a list of parsed conditions and turns them back into Hensel notation.'''
    conditions.sort()
    newconditions = {}
    for x in conditions:
        if len(x) == 2:
            prefix = x
            suffix = ''
        else:
            prefix = x[:2]
            suffix = x[2]
        if prefix not in newconditions:
            newconditions[prefix] = []
        newconditions[prefix].append(suffix)
    rulestring = 'B'
    birth = True
    for x in newconditions:
        if x[0] == 'S' and birth:
            birth = False
            rulestring += '/S'
        num_conditions = len(newconditions[x])
        total_conditions = len(conditiondict[x[1]])
        if num_conditions == total_conditions:
            #No need for letters if the number of conditions matches the total.
            rulestring += x[1]
            continue
        if num_conditions * 2 <= total_conditions:
            #Less than or equal to half of the total conditions are present:
            rulestring += x[1]
            newconditions[x].sort()
            for n in newconditions[x]:
                rulestring += n
            continue
        if num_conditions * 2 > total_conditions:
            #More than half of the total conditions are present.
            missing_conditions = list(set(conditiondict[x[1]]) - set(newconditions[x]))
            missing_conditions.sort()
            rulestring += x[1] + '-'
            for n in missing_conditions:
                rulestring += n
            continue
    if rulestring.count('S') == 0:
        rulestring += '/S'
    return rulestring
def canoniserule(rule):
    '''Canonises a rule to a fixed format.'''
    return tohensel(parserule(rule)).lower().replace('/', '')
def get_9bit(condition):
    '''Returns a list of 9-bit decimal numbers, used to apply INT rules.'''
    #I absolutely hated this part, but at least
    #I was able to automate it partially using
    #another Python script.
    conditions = []
    if condition ==  'B0':
        conditions = [0]
    elif condition ==  'B1e':
        conditions = [2, 8, 32, 128]
    elif condition ==  'B1c':
        conditions = [1, 4, 64, 256]
    elif condition ==  'B2a':
        conditions = [3, 6, 9, 36, 72, 192, 288, 384]
    elif condition ==  'B2e':
        conditions = [10, 34, 136, 160]
    elif condition ==  'B2c':
        conditions = [5, 65, 260, 320]
    elif condition ==  'B2i':
        conditions = [40, 130]
    elif condition ==  'B2k':
        conditions = [12, 33, 66, 96, 129, 132, 258, 264]
    elif condition ==  'B2n':
        conditions = [68, 257]
    elif condition ==  'B3a':
        conditions = [11, 38, 200, 416]
    elif condition ==  'B3c':
        conditions = [69, 261, 321, 324]
    elif condition ==  'B3e':
        conditions = [42, 138, 162, 168]
    elif condition ==  'B3i':
        conditions = [7, 73, 292, 448]
    elif condition ==  'B3j':
        conditions = [14, 35, 74, 137, 164, 224, 290, 392]
    elif condition ==  'B3k':
        conditions = [98, 140, 161, 266]
    elif condition ==  'B3n':
        conditions = [13, 37, 67, 193, 262, 328, 352, 388]
    elif condition ==  'B3q':
        conditions = [70, 76, 100, 196, 259, 265, 289, 385]
    elif condition ==  'B3r':
        conditions = [41, 44, 104, 131, 134, 194, 296, 386]
    elif condition ==  'B3y':
        conditions = [97, 133, 268, 322]
    elif condition ==  'B4a':
        conditions = [15, 39, 75, 201, 294, 420, 456, 480]
    elif condition ==  'B4c':
        conditions = [325]
    elif condition ==  'B4e':
        conditions = [170]
    elif condition ==  'B4i':
        conditions = [45, 195, 360, 390]
    elif condition ==  'B4j':
        conditions = [106, 142, 163, 169, 172, 226, 298, 394]
    elif condition ==  'B4k':
        conditions = [99, 141, 165, 225, 270, 330, 354, 396]
    elif condition ==  'B4n':
        conditions = [71, 77, 263, 293, 329, 356, 449, 452]
    elif condition ==  'B4q':
        conditions = [102, 204, 267, 417]
    elif condition ==  'B4r':
        conditions = [43, 46, 139, 166, 202, 232, 418, 424]
    elif condition ==  'B4t':
        conditions = [105, 135, 300, 450]
    elif condition ==  'B4w':
        conditions = [78, 228, 291, 393]
    elif condition ==  'B4y':
        conditions = [101, 197, 269, 323, 326, 332, 353, 389]
    elif condition ==  'B4z':
        conditions = [108, 198, 297, 387]
    elif condition ==  'B5a':
        conditions = [79, 295, 457, 484]
    elif condition ==  'B5c':
        conditions = [171, 174, 234, 426]
    elif condition ==  'B5e':
        conditions = [327, 333, 357, 453]
    elif condition ==  'B5i':
        conditions = [47, 203, 422, 488]
    elif condition ==  'B5j':
        conditions = [103, 205, 271, 331, 358, 421, 460, 481]
    elif condition ==  'B5k':
        conditions = [229, 334, 355, 397]
    elif condition ==  'B5n':
        conditions = [107, 143, 167, 233, 302, 428, 458, 482]
    elif condition ==  'B5q':
        conditions = [110, 206, 230, 236, 299, 395, 419, 425]
    elif condition ==  'B5r':
        conditions = [109, 199, 301, 361, 364, 391, 451, 454]
    elif condition ==  'B5y':
        conditions = [173, 227, 362, 398]
    elif condition ==  'B6a':
        conditions = [111, 207, 303, 423, 459, 486, 489, 492]
    elif condition ==  'B6c':
        conditions = [175, 235, 430, 490]
    elif condition ==  'B6e':
        conditions = [335, 359, 461, 485]
    elif condition ==  'B6i':
        conditions = [365, 455]
    elif condition ==  'B6k':
        conditions = [231, 237, 363, 366, 399, 429, 462, 483]
    elif condition ==  'B6n':
        conditions = [238, 427]
    elif condition ==  'B7c':
        conditions = [239, 431, 491, 494]
    elif condition ==  'B7e':
        conditions = [367, 463, 487, 493]
    elif condition ==  'B8':
        conditions = [495]
    if len(conditions) > 0:
        return conditions
    if condition[0] == 'S':
        #Since I'm not about to manually add more conditions,
        #I just add 16 to account for the central cell
        #for survival conditions.
        newcon = condition.replace('S', 'B')
        newlist = get_9bit(newcon)
        newlist = [x + 16 for x in newlist]
        return newlist
    return []
def makeconditionarr(rule):
    '''Creates the boolean condition array needed for compilation.'''
    array = [0 for x in range(512)]
    conditions = parserule(rule)
    for x in conditions:
        for y in get_9bit(x):
            array[y] = 1
    return str(array).replace('[', '{').replace(']', '}').replace(' ', '')
def configparams(rule):
    if not isvalid(rule):
        raise ValueError('Rule '+str(rule)+' is not recognised as a non-B0 isotropic Moore rule.')
    #Edit the parameters header file:
    rule = canoniserule(rule)
    sys.stderr.write('Configuring params.hpp for rule '+rule+'...\n')
    slashedrule = rule.replace('b', 'B').replace('s', '/S')
    conditionarr = makeconditionarr(rule)
    params = '''// Core parameters.
#pragma once
#include <string>
std::string rule = "''' + rule + '''";
std::string slashedrule = "''' + slashedrule + '''";
// Used for the actual simulation logic.
// The boolean array's contents will depend upon the rule being simulated.
const bool conditionset[512] = ''' + conditionarr + ';' + '''
#ifndef MAX_PERIOD
#define MAX_PERIOD 1024
#endif
#ifndef umap
#include <unordered_map>
#define umap std::unordered_map
#endif'''
    paramsfile = rootdir + '/includes/params.hpp'
    with open(paramsfile, 'w', encoding='utf-8') as f:
        f.write(params)
    sys.stderr.write('Configuration successful.\n')
if __name__ == '__main__':
    rule = 'b3s23' if len(sys.argv) < 2 else sys.argv[1]
    configparams(rule)
