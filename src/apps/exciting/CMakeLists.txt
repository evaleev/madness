# src/apps/exciting

add_executable(wannier wannier.cc)
target_link_libraries(wannier 
    exciting.a fftlib.a 
    /opt/intel/fce/10.1.015/lib/libifcore.a 
    /opt/intel/fce/10.1.015/lib/libirc.a 
    /opt/intel/fce/10.1.015/lib/libimf.a 
    /opt/intel/fce/10.1.015/lib/libsvml.a
    MADmra)
