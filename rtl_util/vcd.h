#ifndef VCD_H
#define VCD_H
#include <stdint.h>

namespace vcd {
  typedef enum {
    NONE,
    SCOPE,
    SCOPE_TYPE,    
    SCOPE_NAME,
    UPSCOPE,
    VAR,
    VAR_TYPE,
    VAR_SIZE,
    VAR_IDENTIFIER_CODE,
    VAR_REFERENCE,
    VAR_REG_BITS,
    DATA_SECTION,
    SET_TIME_UNIT,
    UPDATE_VAR,
    UPDATE_VAR_BINARY,
    END
  }vcd_parse_state;
}
#endif