/*
   Initialize and run the ORG1411 GPS
*/

void ORG1411Reset(void);
void ORG1411Setup(void);
void ORG1411Handler(void);

void slaveProcess(void);
void TWIMasterMode(TWI_t *twi);
void TWISlaveMode(TWI_t *twi,void (*handler)(void),uint8_t selfAddress);
