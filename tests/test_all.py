'''Unit tests for the Python bindings.'''
import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.dirname(__file__))))
from pocketcpplife import *
reset()
import unittest
class TestPatternMethods(unittest.TestCase):

    def test_creation(self):
        print('Testing creation...')
        #Using my PFP as a test; initialise patterns by RLE and apgcode, then check that they are equal:
        pt = Pattern('''x = 21, y = 21, rule = B3/S23
7b2o3b2o$6bo2bobo2bo$6b2obobob2o$9bobo$6b2obobob2o$7bobobobo$b2obo2bo
bobobo2bob2o$obob3o2bobo2b3obobo$o8bobo8bo$b8o3b8o2$b8o3b8o$o8bobo8bo
$obob3o2bobo2b3obobo$b2obo2bobobobo2bob2o$7bobobobo$6b2obobob2o$9bobo
$6b2obobob2o$6bo2bobo2bo$7b2o3b2o!
''')
        self.assertEqual(pt.population, 136)
        pt2 = Pattern('xs136_y2ml1u0u1lmzcimgmkkjgf0fgjkkmgmiczciq2qaai2s0s2iaaq2qiczy2qb0v0v0bqzy311x11', 'b3s23')
        self.assertEqual(pt2.population, 136)
        #Empty pattern:
        pt3 = Pattern()
        self.assertTrue(pt3.empty())
        #Copying pattern:
        pt4 = Pattern(pt2)
        self.assertEqual(pt2, pt4)
    def test_period(self):
        print('Testing period...')
        #Using the Coe ship as a test:
        pt = Pattern('''x = 10, y = 9, rule = B3/S23
4b6o$2b2o5bo$2obo5bo$4bo3bo$6bo$6b2o$5b4o$5b2ob2o$7b2o!
''', 'b3s23')
        self.assertEqual(pt.apgcode, 'xq16_gcbgzvgg826frc')
        self.assertEqual(pt.period, 16)
        self.assertEqual(pt.displacement, (8, 0))
        #SVG test:
        if os.path.isfile('test.svg'):
            os.remove('test.svg')
        pt.write_svg('test.svg', 400, 400)
        self.assertTrue(os.path.exists('test.svg'))
    def test_advance(self):
        #Lidka as a test and benchmark:
        print('Testing advance...')
        pt = Pattern('''x = 9, y = 15, rule = B3/S23
bo$obo$bo8$8bo$6bobo$5b2obo2$4b3o!
''', 'b3s23')
        self.assertEqual(pt.population, 13)
        evpt = pt[29055]
        self.assertEqual(evpt.population, 1625)
    def test_logic(self):
        #Test boolean pattern comparison and emptiness.
        print('Testing comparison...')
        pt1 = Pattern('''x = 4, y = 5, rule = B3/S23
b2o$3o$2obo$b3o$2bo!
''')
        pt2 = Pattern('xq4_6frc')
        print(pt2.rle_string())
        self.assertEqual(pt1, pt2)
        self.assertTrue(pt1.nonempty())
        self.assertFalse(pt2.empty())
        pt3 = pt2 - pt2
        self.assertTrue(pt3.empty())
    def test_hashsoup(self):
        print('Testing hashsoup...')
        #Test a soup from Travelling Ts (also serves as an OCA test):
        pt = hashsoup('b3s23-a5', 'k_NJ6K9Rkcvj662971047', 'D8_1')
        evsoup = pt[100]
        self.assertEqual(evsoup.period, 17)
        self.assertEqual(evsoup.apgcode, 'xp17_w8k80u1d6b6d1u08k8zciqa24kjge0egjk42aqiczdjmlg8ai2s0s2ia8glmjdzw4a40u1cokoc1u04a4zy31101011')

        #Stdin soup:
        pt2 = hashsoup('b3s23', 'k_w9NBaGgwCBHi-2bo$bobo$o3bo$bo3bo$2bo3bo$3bo3bo$4bobo$5bo6$7bo$7bo$6bobo$7bo$7bo-$7bo$7bo$6bobo$7bo$7bo!', 'PK22_Test_stdin')
        self.assertEqual(pt2.period, 120)
        self.assertEqual(pt2.apgcode, 'xp120_xsssy375777757z777')
    def test_download(self):
        print('Testing download...')
        #Download soups for the 34P14 shuttle:
        soups = download_soups('xp14_033y133zo4maxam4oz1ppy1pp1', 'b3s23')
        soups2 = soups['D2_+1']
        
        #Download the synthesis for a block, since it will never change:
        synth = download_synthesis('xs4_33')
        self.assertTrue(synth.population == 10)
        
if __name__ == '__main__':
    unittest.main()
