#include <raw_api.h>

void bsp_init(void);
void create_init_task(void);

int main()
{
	bsp_init();
	
	raw_os_init();
	
	create_init_task();
	
	raw_os_start();
	
	return 0;
}
