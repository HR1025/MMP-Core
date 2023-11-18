# Hint : not to use the cmake inside eigen project, it's conflict with poco project
#        (add_custom_command uninstall)
# Todo : maybe a better way
include_directories(${MMP_CORE_PRO_TOP_DIR}/Extension/eigen)