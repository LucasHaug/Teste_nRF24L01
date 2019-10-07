#include "mbed.h"
#include "nRF24L01P.h"

//Lê do terminal, manda (tx), recebe de volta (rx) e imprime no terminal

Serial pc(USBTX, USBRX); // tx, rx

nRF24L01P my_nrf24l01p(PA_7, PA_6, PA_5, PC_7, PA_9, PB_6);    // mosi, miso, sck, csn, ce, irq

int main() {

    #define TRANSFER_SIZE   3  //3 bytes na forma de 3 caracteres (3 bytes = 28 bits)

    // Num de bytes tx = num de bytes rx
    char rxData[TRANSFER_SIZE];
    int txDataCnt = 0;   //Bytes enviados
    int rxDataCnt = 0;   //Bytes recebidos


    my_nrf24l01p.powerUp();   //Aciona a NRF24L01+

    my_nrf24l01p.setTransferSize( TRANSFER_SIZE );  //Determina tamanho do payload


    // Mostre as configurações
    pc.printf( "nRF24L01+ Frequencia    : %d MHz\r\n",  my_nrf24l01p.getRfFrequency() );
    pc.printf( "nRF24L01+ Pot. de saida : %d dBm\r\n",  my_nrf24l01p.getRfOutputPower() );
    pc.printf( "nRF24L01+ Taxa          : %d kbps\r\n", my_nrf24l01p.getAirDataRate() );
    pc.printf( "nRF24L01+ End. TX       : 0x%010llX\r\n", my_nrf24l01p.getTxAddress() );
    pc.printf( "nRF24L01+ End. RX       : 0x%010llX\r\n", my_nrf24l01p.getRxAddress() );
    pc.printf( "nRF24L01+ CRC           : %d bits\r\n", my_nrf24l01p.getCrcWidth() );
    pc.printf( "nRF24L01+ Tam. Payload  : %d byes\r\n", my_nrf24l01p.getTransferSize() );


    my_nrf24l01p.setReceiveMode();

    my_nrf24l01p.enable();

    while (1) {

        // Pooling: Verifica se a NRF24L01+ recebeu algo
        if ( my_nrf24l01p.readable() ) {

            // Leia o que está na fila de recepção
            rxDataCnt = my_nrf24l01p.read( NRF24L01P_PIPE_P0, rxData, sizeof( rxData ) );


            // Mostre os 3 bytes recebidos através da terminal conectado na porta serial
            pc.printf("Valores recebidos: %d, %d, %d\r\n", rxData[0],rxData[1],rxData[2]);

        }
    }
}
