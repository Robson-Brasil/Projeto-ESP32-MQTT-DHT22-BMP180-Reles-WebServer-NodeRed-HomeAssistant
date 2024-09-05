# Projeto IoT - Automação Residencial

Este é o meu primeiro projeto de automação residencial utilizando IoT, desenvolvido com o ESP32 Wroom de 38 pinos, sensores DHT22 e BMP180, e um módulo relé de 8 canais. O objetivo foi criar uma solução acessível para controlar dispositivos eletrônicos remotamente e monitorar condições ambientais como temperatura, umidade, pressão atmosférica e altitude.

## Visão Geral

Neste projeto, optei por utilizar um **broker MQTT** rodando em um celular por motivos financeiros. Embora funcione bem na maior parte do tempo, ocasionalmente é necessário reiniciar o servidor MQTT para restaurar a comunicação.

### Componentes Utilizados:
- **ESP32 Wroom (38 pinos)**
- **Sensor de Temperatura e Umidade DHT22**
- **Sensor de Pressão BMP180**
- **Módulo Relé de 8 Canais**
- **Broker MQTT** (rodando em dispositivo móvel)

## Tecnologias Integradas

O projeto faz uso do protocolo **MQTT** para comunicação entre dispositivos, integrando-se com diversas plataformas, incluindo o **Node-RED**, **Google Assistant (via SmartNora)** e **Home Assistant**.

### Ferramentas Usadas:

- **Node-RED** e **Google Assistant** (via [SmartNora](https://smart-nora.eu))
  - Tutorial de configuração do SmartNora: [YouTube](https://www.youtube.com/watch?v=cK8-hTdW2Kg&t=752s)
  - Outro tutorial útil do SmartNora: [YouTube](https://www.youtube.com/watch?v=irrOqCr7UMY)

- **Home Assistant**:
  - Instalação do Home Assistant: [Documentação Oficial](https://www.home-assistant.io/installation/)

- **Node-RED**:
  - Instalação do Node-RED: [Instruções de Instalação](https://nodered.org/docs/getting-started/)

## Configurações do Hardware

### Ligações na Protoboard:
![Projeto de IoT](https://github.com/user-attachments/assets/44182bec-8cb9-4c78-b2b9-41a09ed213f8)

## Dashboards Utilizadas

O projeto conta com painéis de controle móveis para monitoramento e acionamento de dispositivos. Abaixo estão exemplos das telas utilizadas:

- **Virtuino6 MQTT (Relés e DHT22)**:
  ![Screenshot_20240905-153253_Virtuino6](https://github.com/user-attachments/assets/3e5bca48-1559-4dd4-8305-67f7b9377dd9)

- **Virtuino6 MQTT (BMP180)**:
  ![Screenshot_20240905-153347_Virtuino6](https://github.com/user-attachments/assets/e06621c2-2a99-4a30-9d99-bac14ab401ea)

- **Home Assistant (Desatualizado)**:
  ![Screenshot_20220830-233639](https://user-images.githubusercontent.com/66142021/187596337-b61de278-251a-4ec3-80ad-c39e651a6c50.png)
