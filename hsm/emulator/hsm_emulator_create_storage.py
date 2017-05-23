#!/usr/bin/env python
import argparse
import random
import struct

from ecdsa import ecdsa


MAXINT32 = (1 << 32) - 1

def main():
    parser = argparse.ArgumentParser(prog='create_hsm_fake_storage')
    parser.add_argument('-n', '--num-reqs', type=int,
                        default=256,
                        help='requested number of pairs <private_key, public_key>')
    parser.add_argument('-o', '--output', type=argparse.FileType('w'),
                        required=True,
                        help='output file path')
    parser.add_argument('-c', '--curve', type=int,
                        default=256, choices=[224, 256],
                        help='curve type')
    args = parser.parse_args()

    def get_cal_format(value):
        words = []
        while value:
            words.append(value & MAXINT32)
            value >>= 32 
        return  '{%s}' % (', '.join('0x%x' % w for w in words))
        
    def dump_tuple_private_public_key(private_key, pub_key_x, pub_key_y):
       pair = '{{%s}, {%s, \n{%s}, \n{%s}}}' % (get_cal_format(private_key), 
                                      'ECC_POINT_UNCOMPRESSED', 
                                      get_cal_format(pub_key_x), 
                                      get_cal_format(pub_key_y))
       return pair        

    def dump_all_keys():  
        private_key = 0
        pairs = []     
        for _ in xrange(args.num_reqs):
            private_key += 1
            
            if args.curve == 224:
                private_key_obj = ecdsa.Private_key(ecdsa.generator_224, private_key)
            else:
                private_key_obj = ecdsa.Private_key(ecdsa.generator_256, private_key)
                
            product = private_key_obj.generator * private_key_obj.secret_multiplier
            public_key_obj = ecdsa.Public_key(private_key_obj.generator, product)
        
            pair = dump_tuple_private_public_key(private_key, 
                                                public_key_obj.point.x(),
                                                public_key_obj.point.y())
            pairs.append(pair)
        
        all = '%s' % (',\n'.join('%s' % p for p in pairs))
        args.output.write(all)

    def make_header_file():
        args.output.write('/* Do not change - auto generated code */\n\n')
        args.output.write('#include \"hsm_fake_storage.h\"\n\n')
        args.output.write('static hsm_fake_storage_entry_t hsm_fake_storage_p%d[HSM_FAKE_STORAGE_SIZE] = {\n' % args.curve)
        dump_all_keys()
        args.output.write('};\n')
    
    make_header_file()

if __name__ == '__main__':
    main()
