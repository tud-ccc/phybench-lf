

target_include_directories(${LF_MAIN_TARGET} PUBLIC "/${CMAKE_CURRENT_LIST_DIR}/phybench")
target_include_directories(${LF_MAIN_TARGET} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/ulRsGen/inc")
target_include_directories(${LF_MAIN_TARGET} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/ulRsGen/common/inc")

target_sources(${LF_MAIN_TARGET} PUBLIC
# kernels
"${CMAKE_CURRENT_LIST_DIR}/phybench/ant_comb_7.cc"
"${CMAKE_CURRENT_LIST_DIR}/phybench/chest_5.cc"
"${CMAKE_CURRENT_LIST_DIR}/phybench/complex_def.cc"
"${CMAKE_CURRENT_LIST_DIR}/phybench/crc_13.cc"
"${CMAKE_CURRENT_LIST_DIR}/phybench/fft_8.cc"
"${CMAKE_CURRENT_LIST_DIR}/phybench/interleave_11.cc"
"${CMAKE_CURRENT_LIST_DIR}/phybench/mf_4.cc"
"${CMAKE_CURRENT_LIST_DIR}/phybench/mmse_by_cholsolve_4xX_complex.cc"
"${CMAKE_CURRENT_LIST_DIR}/phybench/soft_demap_9.cc"
"${CMAKE_CURRENT_LIST_DIR}/phybench/turbo_dec_12.cc"
"${CMAKE_CURRENT_LIST_DIR}/phybench/uplink_parameters.cc"
#"${CMAKE_CURRENT_LIST_DIR}/phybench/uplink_user.cc"
"${CMAKE_CURRENT_LIST_DIR}/phybench/weight_calc_6.cc"

# ulRsGen
"${CMAKE_CURRENT_LIST_DIR}/ulRsGen/src/alpha_n_prs.cc"
"${CMAKE_CURRENT_LIST_DIR}/ulRsGen/src/n_prs_all_ns.cc"
"${CMAKE_CURRENT_LIST_DIR}/ulRsGen/src/r_alpha.cc"
"${CMAKE_CURRENT_LIST_DIR}/ulRsGen/src/ulRsGen.cc"
"${CMAKE_CURRENT_LIST_DIR}/ulRsGen/common/src/feldspar.cc"
"${CMAKE_CURRENT_LIST_DIR}/ulRsGen/common/src/missingFunc.cc"
"${CMAKE_CURRENT_LIST_DIR}/ulRsGen/common/src/feldspar_functions.cc"
)

