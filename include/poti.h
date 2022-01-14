
#include <MCP41HV.h>
#include <pin_map.h>

MCP41HV & get_handle_poti();

#define poti get_handle_poti()

void poti_init();