#include "MKL46Z4.h"

#include "I2C.h"

//żółwik działa ^^

void i2c_Disable(I2C_Type* i2c){
  i2c->C1 &= ~I2C_C1_IICEN_MASK;
}

void i2c_Disable_Interrupt(I2C_Type* i2c){
  i2c->C1 &= ~I2C_C1_IICIE_MASK;
}

void i2c_Enable(I2C_Type* i2c){
  i2c->C1 |= I2C_C1_IICEN_MASK;
}

void i2c_Enable_Interrupt(I2C_Type* i2c){
  i2c->C1 |= I2C_C1_IICIE_MASK;
}

uint8_t i2c_ReadByte(I2C_Type* i2c, uint8_t ack){
  // Wybierz tryb odbioru
  i2c->C1 &= ~I2C_C1_TX_MASK;
  
  // Przygotuj bit ACK/NACK jeżeli FACK == 0
  if((i2c->SMB & I2C_SMB_FACK_MASK) == 0)
    i2c->C1 = (ack == I2C_NACK) ? i2c->C1 | I2C_C1_TXAK_MASK : i2c->C1 & ~I2C_C1_TXAK_MASK;
  
  // Wyczyść flagę IICIF
  i2c->S |= I2C_S_IICIF_MASK;
  
  // Inicjalizacja transferu danych
  (void)i2c->D;					//nie wiem dlaczemu tak ale internet mówi że tak trzeba
  // Oczekiwanie na zakończenie transferu
  while((i2c->S & I2C_S_IICIF_MASK) == 0);
  
  // Wyślij bit ACK/NACK jeżeli FACK == 1
  if((i2c->SMB & I2C_SMB_FACK_MASK) != 0)
    i2c->C1 = (ack == I2C_NACK) ? i2c->C1 | I2C_C1_TXAK_MASK : i2c->C1 & ~I2C_C1_TXAK_MASK;
  
  // Przejście w tryb transmisji
  i2c->C1 |= I2C_C1_TX_MASK;
  
  // Zwróc odebrane dane
  return i2c->D;
}

void i2c_Restart(I2C_Type* i2c){
  i2c->C1 |= I2C_C1_RSTA_MASK;
}

void i2c_Start(I2C_Type* i2c){
  i2c->C1 |= I2C_C1_MST_MASK;
}

void i2c_Stop(I2C_Type * i2c){
  // Wyczyść flagę STOP
  i2c->FLT |= I2C_FLT_STOPF_MASK;
  
  // Wpisz bit stopu
  i2c->C1 &= ~I2C_C1_MST_MASK;
  // Oczekiwanie na zakończenie transmisji bitu stopu
  while((i2c->FLT & I2C_FLT_STOPF_MASK) == 0){
    i2c->C1 &= ~I2C_C1_MST_MASK;
  }
}

uint8_t i2c_WriteByte(I2C_Type* i2c, uint8_t data){
  // Przejście w tryb transmisji
  i2c->C1 |= I2C_C1_TX_MASK;
  
  // Wyczyść flagę IICIF
  i2c->S |= I2C_S_IICIF_MASK;
  
  // Inicjalizacja transferu danych
  i2c->D = data;
  // Oczekiwanie na zakończenie transferu
  while((i2c->S & I2C_S_IICIF_MASK) == 0);
  
  // Zwróć otrzymany bit ACK
  return ((i2c->S & I2C_S_RXAK_MASK) == I2C_S_RXAK_MASK ? I2C_NACK : I2C_ACK);
}
