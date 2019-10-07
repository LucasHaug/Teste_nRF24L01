#include "mbed.h"
#include "nRF24L01P.h"

//Lê do terminal, manda (tx), recebe de volta (rx) e imprime no terminal

Serial pc(USBTX, USBRX); // tx, rx

nRF24L01P my_nrf24l01p(PA_7, PA_6, PA_5, PC_7, PA_9, PB_6);    // mosi, miso, sck, csn, ce, irq

int main() {

    #define TRANSFER_SIZE   3  //3 bytes na forma de 3 caracteres (3 bytes = 28 bits)

    char txData[TRANSFER_SIZE]={10, 21, 32};
    int txDataCnt = TRANSFER_SIZE;
    unsigned int contador=0;

    my_nrf24l01p.powerUp();   //Aciona a NRF24L01+

    my_nrf24l01p.setTransferSize( TRANSFER_SIZE );  //Determina tamanho do payload


    // Mostre as configurações
    pc.printf( "nRF24L01+ Frequencia    : %d MHz\r\n",  my_nrf24l01p.getRfFrequency() );
    pc.printf( "nRF24L01+ Pot. de saida : %d dBm\r\n",  my_nrf24l01p.getRfOutputPower() );
    pc.printf( "nRF24L01+ Taxa          : %d kbps\r\n", my_nrf24l01p.getAirDataRate() );
    pc.printf( "nRF24L01+ End. TX       : 0x%010llX\r\n", my_nrf24l01p.getTxAddress() );
    pc.printf( "nRF24L01+ End. RX       : 0x%010llX\r\n", my_nrf24l01p.getRxAddress() );
    pc.printf( "nRF24L01+ CRC           : %d bits\r\n", my_nrf24l01p.getCrcWidth() );
    pc.printf( "nRF24L01+ Tam. Payload  : %d bytes\r\n", my_nrf24l01p.getTransferSize() );


    my_nrf24l01p.setReceiveMode();   //O TX mode é ativado quando se usar a função de escrita

    my_nrf24l01p.enable();

    while (1) {

            // Envia o conteúdo de txData via nRF24L01+
            int teste = my_nrf24l01p.write( NRF24L01P_PIPE_P0, txData, txDataCnt );

            pc.printf("write = %d\r\n", teste);

            contador++;

            pc.printf("Enviei %d, %d, %d pela %d vez \r\n", txData[0],txData[1],txData[2],contador);

            wait(0.8);  //Espera 0.8s para transmitir

    }
}
