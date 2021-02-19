cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(pico-serial)
pico_sdk_init()
add_executable(pico-serial
  pico-serial.c
  ../../../../ose.c ../../../../ose_util.c ../../../../ose_stackops.c 
  ../../../../ose_context.c ../../../../ose_vm.c ../../../../ose_match.c
  ../../../../ose_builtins.c ../../../../ose_symtab.c
  )
pico_enable_stdio_usb(pico-serial 1)
# pico_enable_stdio_uart(pico-serial 1)
pico_add_extra_outputs(pico-serial)
target_link_libraries(pico-serial pico_stdlib hardware_adc)
target_include_directories(pico-serial PRIVATE ../../../../)
add_definitions(
-DOSEVM_ASSIGN=my_assign
-DOSEVM_LOOKUP=my_lookup
-DOSEVM_ISKNOWNADDRESS=my_isKnownAddress
-DOSEVM_DEFAULT=my_default
#   -DOSE_CONF_VM_INPUT_SIZE=1024
#   -DOSE_CONF_VM_STACK_SIZE=1024
#   -DOSE_CONF_VM_ENV_SIZE=1024
#   -DOSE_CONF_VM_CONTROL_SIZE=1024
#   -DOSE_CONF_VM_DUMP_SIZE=1024
#   -DOSE_CONF_VM_OUTPUT_SIZE=1024
)
