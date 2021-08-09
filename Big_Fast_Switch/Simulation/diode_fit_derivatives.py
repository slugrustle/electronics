#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Equation for diode external forward voltage (VAK) given various
# parameters used in SPICE simulators and partial derivatives
# of this equation with respect to these parameters.
from sympy import *

N, k, T, q, ID, Is, Rs, VAK = symbols('N, k, T, q, ID, Is, Rs, VAK', real=True, positive=True)
pdiff_Is, pdiff_N, pdiff_Rs = symbols('pdiff_Is, pdiff_N, pdiff_Rs')

VAK = (N*k*T/q) * log(ID/Is + 1) + Rs * ID
print('VAK = ', VAK)

pdiff_Is = diff(VAK, Is)
print('dVAK/dIs = ', pdiff_Is)

pdiff_N = diff(VAK, N)
print('dVAK/dN', pdiff_N)

pdiff_Rs = diff(VAK, Rs)
print('dVAK/dRs', pdiff_Rs)

