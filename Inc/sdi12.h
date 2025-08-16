

#define sdi_pin_read (GPIOE->IDR & GPIO_PIN_8)
#define sdi_pin_set (GPIOE->ODR |= GPIO_PIN_8)
#define sdi_pin_reset (GPIOE->ODR &= ~GPIO_PIN_8)

#define sdi_port GPIOE 
#define sdi_pin 8