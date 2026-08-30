'''Functions for engaging with Catagolue and payosha256.'''
import urllib.request
import os
import hashlib
import sys
CATAGOLUE_URL = 'https://catagolue.hatsya.com'
def checkconnection():
    '''Checks if it is possible to connect to Catagolue.'''
    try:
        urllib.request.urlopen(CATAGOLUE_URL)
        return True
    except Exception as e:
        print(e)
        return False
def authenticate(operation, key='#anon'):
    '''Authenticates using payosha256.'''
    if not checkconnection():
        return None
    #Get a token from payosha256:
    payload = 'payosha256:get_token:'+key+':'+operation
    payload = payload.encode('utf-8')
    req = urllib.request.Request(CATAGOLUE_URL + '/payosha256', payload, {'Content-type': 'text/plain'})
    c = urllib.request.urlopen(req)
    response = c.read().splitlines()
    #Compute the hash puzzle for the received token:
    for x in response:
        parts = x.decode('utf-8').split(':')
        if len(parts) < 3:
            continue
        if parts[1] != 'good':
            continue
        target, token = parts[2], parts[3]
        for nonce in range(10000000):
            prehash = token + ':' + str(nonce)
            posthash = hashlib.sha256(prehash.encode('utf-8')).hexdigest()
            if posthash < target:
                break
        if posthash > target:
            continue
        payload = 'payosha256:pay_token:'+prehash+'\n'
        return payload
    return None
def upload_results(results, key='#anon', operation='post_apgsearch_haul', endpoint='/apgsearch', return_point = None):
    '''Uploads search results to Catagolue.
Accepts either a string or filename as input.'''
    if os.path.isfile(results):
        with open(results, 'r', encoding='utf-8') as f:
            results = f.read()
    payload = authenticate(operation, key)
    if payload is None:
        return 1
    payload += results
    req = urllib.request.Request(CATAGOLUE_URL + endpoint,
                                 payload.encode('utf-8'),
                                 {'Content-type': 'text/plain'})
    with urllib.request.urlopen(req) as f:
        if f.getcode() != 200:
            return 2
        response = f.read().decode('utf-8')
        sys.stderr.write(response)
        if return_point is not None:
            return_point = resp.decode('utf-8')
    if return_point is not None:
        return_point[0] = resp
    return 0
if __name__ == '__main__':
    file = sys.argv[1]
    key = '#anon' if len(sys.argv) < 3 else sys.argv[2]
    upload_results(file, key)
