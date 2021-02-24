# -*- coding: utf-8 -*-

#variable = 'value'
#variable_bool = 'yes' | 'no'

prefix = '/usr'
sysconfdir = '/etc'
with_warnerrors = 'no'

# determines if the application is linked against the FOX-1.6 DLL/so or
# against the static fox library. if 'yes' FOX-1.6.dll (and png.dll,
# jpeg.dll and z.dll depending on the FOX build) have to be accessible
# to the executable. if 'no' libfox-1.6_static, libpng_static,
# libjpeg_static and libz_static libraries have to be available. in this
# case no additional DLL/so are required.
with_foxstatic = 'no'
