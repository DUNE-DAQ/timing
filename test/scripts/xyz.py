#!/usr/bin/env python

from __future__ import print_function

class A(object):
    """docstring for A"""
    def __init__(self):
        super(A, self).__init__()

    def a(self):
        print('aaa',self.x)


class B(object):
    """docstring for A"""
    def __init__(self):
        super(B, self).__init__()

    def b(self):
        print('bbb',self.x)

if __name__ == '__main__':
    def initme(self, x):
        self.x = x
    
    C = type('C',(A, B),{'__init__':initme})
    print (C)

    c = C('x')
    c.a()
    c.b()
