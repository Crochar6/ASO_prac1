/**
 * @file   main.c
 * @author Carles Roch Arnau
 * @date   12 Octubre 2021
 * @brief  Fitxer principal de la pràctica 1 de ASO. Rep inputs de botons
   i executa els scripts necessaris
*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>

#define DEBOUNCE_MS 1000

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Carles Roch Arnau");
MODULE_DESCRIPTION("Driver per la practica 1 de aso");
MODULE_VERSION("0.1");

static int irq1, irq2, irq3, irq4;
static int led1 = 20, led2 = 16;
static int button1 = 26, button2 = 19, button3 = 13, button4 = 21;
static int historialBoto = 0;

static irq_handler_t  button1_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs);
static irq_handler_t  button2_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs);
static irq_handler_t  button3_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs);
static irq_handler_t  button4_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs);

static char* argv1[] = {"/home/pi/Documents/CustomScripts/button1.sh", NULL};
static char* argv2[] = {"/home/pi/Documents/CustomScripts/button2.sh", NULL};
static char* argv3[] = {"/home/pi/Documents/CustomScripts/button3.sh", NULL};
static char* argv4[] = {"/home/pi/Documents/CustomScripts/button4.sh", NULL};
static char* envp[] = {"PATH=/sbin:/usr/sbin:/bin:/usr/bin", NULL};

/** @brief Configura els irq i gpio
 *
 */
static int __init lkm_init(void){
	int result = 0;
	printk(KERN_INFO "LKM inicialització\n");
	// Comprobem que els gpio dels leds siguin vàlids
	if (!gpio_is_valid(led1)){
		printk(KERN_INFO "El led 1 és invàlid\n");
		return -ENODEV;
	}
	if (!gpio_is_valid(led2)){
		printk(KERN_INFO "El led 2 és invàlid\n");
		return -ENODEV;
	}
	// Leds
	gpio_request(led1, "sysfs");          		// Request del gpio
	gpio_direction_output(led1, true);   	// Gpio en output
	gpio_export(led1, false);             	// Faig que apareixi a /sys/class/gpio
	gpio_request(led2, "sysfs");          		// Request del gpio
	gpio_direction_output(led2, true);   	// Gpio en output
	gpio_export(led2, false);             	// Faig que apareixi a /sys/class/gpio
	
	// Botó 1
	gpio_request(button1, "sysfs");
	gpio_direction_input(button1);
	gpio_set_debounce(button1, DEBOUNCE_MS);	// Configurem rebots
	gpio_export(button1, false);
	
	// Botó 2
	gpio_request(button2, "sysfs");
	gpio_direction_input(button2);
	gpio_set_debounce(button2, DEBOUNCE_MS);
	gpio_export(button2, false);
	
	// Botó 3
	gpio_request(button3, "sysfs");
	gpio_direction_input(button3);
	gpio_set_debounce(button3, DEBOUNCE_MS);
	gpio_export(button3, false);
	
	// Botó 4
	gpio_request(button4, "sysfs");
	gpio_direction_input(button4);
	gpio_set_debounce(button4, DEBOUNCE_MS);
	gpio_export(button4, false);
	
	printk(KERN_INFO "Estat del boto1: %d\n", gpio_get_value(button1));
	irq1 = gpio_to_irq(button1);
	printk(KERN_INFO "Botó mapejat a IRQ: %d\n", irq1);
	result = request_irq(irq1,
                        (irq_handler_t) button1_irq_handler,
                        IRQF_TRIGGER_FALLING,
                        "button1_gpio_handler",
                        NULL);
						
	printk(KERN_INFO "Estat del boto12: %d\n", gpio_get_value(button2));
	irq2 = gpio_to_irq(button2);
	printk(KERN_INFO "Botó mapejat a IRQ: %d\n", irq2);
	result = request_irq(irq2,
                        (irq_handler_t) button2_irq_handler,
                        IRQF_TRIGGER_FALLING,
                        "button2_gpio_handler",
                        NULL);
	
	printk(KERN_INFO "Estat del boto13: %d\n", gpio_get_value(button3));
	irq3 = gpio_to_irq(button3);
	printk(KERN_INFO "Botó mapejat a IRQ: %d\n", irq3);
	result = request_irq(irq3,
                        (irq_handler_t) button3_irq_handler,
                        IRQF_TRIGGER_FALLING,
                        "button3_gpio_handler",
                        NULL);
	
	printk(KERN_INFO "Estat del boto14: %d\n", gpio_get_value(button4));
	irq4 = gpio_to_irq(button4);
	printk(KERN_INFO "Botó mapejat a IRQ: %d\n", irq4);
	result = request_irq(irq4,
                        (irq_handler_t) button4_irq_handler,
                        IRQF_TRIGGER_FALLING,
                        "button4_gpio_handler",
                        NULL);
	return result;
}

/** @brief Per netejar LKM
 *
 */
static void __exit lkm_exit(void){
	printk(KERN_INFO "El botó s'ha presionat %d cops\n", historialBoto);
	
	// Apaguem els leds
	printk(KERN_INFO "Apagant leds\n");
	gpio_set_value(led1, 0);
	gpio_set_value(led2, 0);
	
	// Unexport leds
	printk(KERN_INFO "Unexport dels leds\n");
	gpio_unexport(led1);
	gpio_unexport(led2); 
	
	// Alliberem les irq
	printk(KERN_INFO "Free dels IRQ\n");
	free_irq(irq1, NULL);
	free_irq(irq2, NULL);
	free_irq(irq3, NULL);
	free_irq(irq4, NULL);
	
	// Unexport botons
	printk(KERN_INFO "Unexport dels botons\n");
	gpio_unexport(button1);
	gpio_unexport(button2);
	gpio_unexport(button3);
	gpio_unexport(button4);
	
	// Free gpio
	printk(KERN_INFO "Free dels gpio\n");
	gpio_free(led1);
	gpio_free(led2);
	gpio_free(button1);
	gpio_free(button2);
	gpio_free(button3);
	gpio_free(button4);
	
	printk(KERN_INFO "LKM tancat, gràcies!\n");
}

/** @brief Funció de la interrupció causada pel botó 1
 *
 */
static irq_handler_t button1_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs){
	gpio_set_value(led1, true);
	
	call_usermodehelper(argv1[0], argv1, envp, UMH_NO_WAIT);
	
	printk(KERN_INFO "Botó 1! \n");
	historialBoto++;
	return (irq_handler_t) IRQ_HANDLED;
}

/** @brief Funció de la interrupció causada pel botó 1
 *
 */
static irq_handler_t button2_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs){
	gpio_set_value(led1, false);
	
	call_usermodehelper(argv2[0], argv2, envp, UMH_NO_WAIT);
	
	printk(KERN_INFO "Botó 2! \n");
	historialBoto++;
	return (irq_handler_t) IRQ_HANDLED; 
}

/** @brief Funció de la interrupció causada pel botó 1
 *
 */
static irq_handler_t button3_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs){
	gpio_set_value(led2, true);
	
	call_usermodehelper(argv3[0], argv3, envp, UMH_NO_WAIT);
	
	printk(KERN_INFO "Botó 3! \n");
	historialBoto++;
	return (irq_handler_t) IRQ_HANDLED;
}

/** @brief Funció de la interrupció causada pel botó 1
 *
 */
static irq_handler_t button4_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs){
	gpio_set_value(led2, false);
	
	call_usermodehelper(argv4[0], argv4, envp, UMH_NO_WAIT);
	
	printk(KERN_INFO "Botó 4! \n");
	historialBoto++;
	return (irq_handler_t) IRQ_HANDLED;
}

// Configurar funcions de init i exit 
module_init(lkm_init);
module_exit(lkm_exit);