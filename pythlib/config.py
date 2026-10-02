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
