'''
'''
# This code is an autocomplete-hint for IDE.
# The object imported here will be overwritten by _load() function.
try: from _lamonpy import *
except: pass

def _load():
    import importlib, os
    env_setting = os.environ.get('LAMONPY_ISA', '').lower().split(',')
    if not env_setting[0]: env_setting = []
    isas = ['avx2', 'avx', 'sse2', 'none']
    if env_setting: isas = [i for i in isas if i in env_setting]
    if len(isas) > 1:
        from cpuinfo import get_cpu_info
        flags = get_cpu_info()['flags']
        isas = [isa for isa in isas if (isa in flags or isa == 'none')]
        if not isas: raise RuntimeError("No isa option for " + str(env_setting))
    for isa in isas:
        try:
            mod_name = '_lamonpy' + ('_' + isa if isa != 'none' else '')
            globals().update({k:v for k, v in vars(importlib.import_module(mod_name)).items() if not k.startswith('_')})
            return
        except:
            if isa == isas[-1]: raise
_load()
del _load
