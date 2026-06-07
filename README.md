# 🌧️🛰️ ChuvaViva — Estação de Alerta de Enchentes e Deslizamentos

Estação embarcada de monitoramento ambiental baseada em **Arduino UNO**, desenvolvida como entrega da **Global Solution 2026 (Space Connect) — FIAP**. O sistema lê variáveis ambientais (nível da água, chuva, umidade do solo e inclinação do terreno), calcula um **índice de risco** ponderado e dispara alertas visuais, sonoros e físicos conforme a gravidade da situação.

---

## 📑 Sumário

1. [Contexto: Global Solution 2026 — Space Connect](#-contexto-global-solution-2026--space-connect)
2. [Descrição do projeto](#-descrição-do-projeto)
3. [Objetivo da solução](#-objetivo-da-solução)
4. [Componentes utilizados](#-componentes-utilizados)
5. [Estrutura do circuito](#-estrutura-do-circuito)
6. [Explicação do funcionamento](#-explicação-do-funcionamento)
7. [Instruções de execução](#-instruções-de-execução)
8. [Link da simulação](#-link-da-simulação)
9. [Integrantes do grupo](#-integrantes-do-grupo)

---

## 🛰️ Contexto: Global Solution 2026 — Space Connect

A Global Solution 2026 da FIAP — edição **Space Connect** — propõe usar tecnologia, dados e inovação para **resolver problemas reais da Terra a partir do conhecimento gerado pela exploração espacial**.

Uma das premissas do desafio é que **ambientes extremos produzem soluções melhores**: o espaço é o ambiente mais restritivo que existe, e boa parte das tecnologias criadas para sobreviver a ele acabou transformando a vida no nosso planeta — do GPS aos filtros de água, da telemedicina aos **sistemas de monitoramento e alerta precoce de desastres naturais**.

É nesse último ponto que o ChuvaViva se conecta ao tema. Um dos maiores legados da exploração espacial é a capacidade de **observar a Terra para prever desastres** — enchentes, furacões, deslizamentos — e emitir alertas que salvam vidas. O ChuvaViva traz essa mesma lógica para o nível local: uma estação autônoma que monitora as condições de risco diretamente no terreno e dispara alertas automáticos **antes** que a tragédia aconteça.

---

## 📋 Descrição do projeto

O **ChuvaViva** é uma estação autônoma de monitoramento que avalia, em tempo real, o risco de enchentes e deslizamentos em uma localidade. A partir da leitura de quatro variáveis ambientais, o dispositivo calcula uma pontuação de risco e classifica a situação em quatro níveis: **BAIXO**, **ATENÇÃO**, **ALERTA** e **CRÍTICO**.

Cada nível aciona um conjunto diferente de respostas:

- **LEDs** indicando o status atual (verde, amarelo ou vermelho).
- **Buzzer** com padrões sonoros distintos para alerta e situação crítica.
- **Servo motor** que atua como uma cancela/barreira física, fechando o acesso à área em risco crítico.
- **Display LCD** exibindo a classificação e uma orientação à população.

O nível da água é medido fisicamente por um sensor ultrassônico. As demais variáveis (chuva, umidade do solo e inclinação) são simuladas por potenciômetros, representando sensores que seriam usados em uma implementação real.

---

## 🎯 Objetivo da solução

Reduzir o tempo de resposta a eventos climáticos extremos por meio de **alertas antecipados e automáticos**, ajudando comunidades vulneráveis a tomar decisões rápidas (como evacuar ou evitar uma área alagada) antes que o desastre se concretize.

A proposta atende diretamente questões ligadas a:

- Prevenção de desastres naturais (enchentes e deslizamentos);
- Segurança da população em áreas de risco;
- Automação de respostas (sinalização e bloqueio físico de acesso) sem depender de intervenção humana imediata.

---

## 🧩 Componentes utilizados

| Componente | Função no projeto |
|---|---|
| Arduino UNO | Microcontrolador central que processa as leituras e controla os atuadores |
| Sensor ultrassônico HC-SR04 | Mede o nível da água (quanto mais próximo, maior o risco) |
| Potenciômetro (x1) | Simula o **sensor de chuva** |
| Potenciômetro (x1) | Simula o **sensor de umidade do solo** |
| Potenciômetro (x1) | Simula o **sensor de inclinação do terreno** |
| LED verde | Indica status **BAIXO** |
| LED amarelo | Indica status **ATENÇÃO** |
| LED vermelho | Indica status **ALERTA** (piscando) e **CRÍTICO** (fixo) |
| Buzzer (piezo) | Emite alertas sonoros |
| Servo motor | Atua como cancela/barreira física |
| Display LCD 16x2 com módulo I2C | Exibe a classificação de risco e orientações |
| Botão (push button) | Reset / confirmação manual (registra evento no Serial Monitor) |
| Protoboard, jumpers e resistores | Montagem e conexões do circuito |

### Bibliotecas necessárias

```cpp
#include <Wire.h>               // Comunicação I2C
#include <LiquidCrystal_I2C.h>  // Controle do display LCD via I2C
#include <Servo.h>              // Controle do servo motor
```

---

## 🔌 Estrutura do circuito

### Mapeamento de pinos

| Componente | Pino do Arduino |
|---|---|
| Botão | D2 (`INPUT_PULLUP`) |
| HC-SR04 — TRIG | D3 |
| HC-SR04 — ECHO | D4 |
| LED verde | D5 |
| LED amarelo | D6 |
| LED vermelho | D7 |
| Buzzer | D8 |
| Servo motor | D9 |
| Potenciômetro — chuva | A0 |
| Potenciômetro — umidade do solo | A1 |
| Potenciômetro — inclinação | A2 |
| Display LCD I2C | SDA (A4) / SCL (A5) |

> **Observação sobre o LCD:** o display I2C está configurado no endereço **`0x20`** (padrão do componente no Tinkercad). Caso o hardware físico utilize outro endereço (comumente `0x27`), basta ajustar a linha `LiquidCrystal_I2C lcd(0x20, 16, 2);` no código.

### Layout

A montagem na protoboard segue, da esquerda para a direita: servo motor, sensor ultrassônico HC-SR04, buzzer, os três LEDs (verde, amarelo e vermelho), os três potenciômetros e o display LCD I2C, todos conectados ao Arduino UNO. A imagem da simulação no Tinkercad ilustra todas as ligações.

---

## ⚙️ Explicação do funcionamento

### 1. Leitura dos sensores

A cada ciclo do `loop()`, o sistema realiza quatro leituras, todas normalizadas para uma escala de **0 a 100**:

- **Nível da água:** o HC-SR04 mede a distância até a superfície da água. A distância (entre 5 cm e 100 cm) é convertida em risco de forma **inversa** — quanto menor a distância, mais alto o nível e maior o risco.
- **Chuva, umidade do solo e inclinação:** lidos diretamente dos potenciômetros (`analogRead` de 0–1023, mapeado para 0–100).

### 2. Cálculo do risco

O índice de risco é uma **média ponderada** das quatro variáveis, priorizando o nível da água:

```
risco = (nível × 0,40) + (chuva × 0,25) + (umidade × 0,25) + (inclinação × 0,10)
```

O resultado é um valor de **0 a 100**.

### 3. Classificação dos estados

| Faixa de risco | Estado | LED | Buzzer | Cancela (servo) | Mensagem no LCD |
|---|---|---|---|---|---|
| `risco < 30` | **BAIXO** | Verde (fixo) | Silêncio | Aberta | `RISCO BAIXO` / `Monitorando...` |
| `30 ≤ risco < 55` | **ATENÇÃO** | Amarelo (fixo) | Silêncio | Aberta | `ATENCAO` / `Acompanhe o nivel` |
| `55 ≤ risco < 75` | **ALERTA** | Vermelho (piscando) | Pulsante (1000 Hz) | Aberta | `ALERTA ENCHENTE` / `Evite a area` |
| `risco ≥ 75` | **CRÍTICO** | Vermelho (fixo) | Contínuo (1500 Hz) | **Fechada (90°)** | `RISCO CRITICO` / `SAIA DA AREA` |

### 4. Atuadores

- **LEDs:** indicam visualmente o nível atual. No estado ALERTA, o LED vermelho pisca em intervalos de 500 ms (controlados por `millis()`, sem travar o restante do programa).
- **Buzzer:** silencioso em situações normais, pulsa no estado ALERTA e mantém som contínuo no estado CRÍTICO.
- **Servo motor:** mantém a cancela aberta (0°) na maior parte do tempo e a fecha (90°) apenas em risco CRÍTICO, bloqueando fisicamente o acesso à área.
- **Display LCD:** só é atualizado quando o estado muda (evita o "piscar" da tela e a reescrita desnecessária do display).
- **Botão:** quando pressionado, registra um evento de reset/confirmação no Serial Monitor (a 9600 baud).

---

## ▶️ Instruções de execução

### Opção 1 — Simulação no Tinkercad (recomendada)

1. Acesse o link da simulação (na seção [Link da simulação](#-link-da-simulação)).
2. Clique em **"Iniciar Simulação"**.
3. Gire os potenciômetros e ajuste o sensor ultrassônico para variar as leituras e observe as mudanças de estado nos LEDs, no buzzer, no servo e no display.

### Opção 2 — Montagem física (Arduino real)

1. Instale a **Arduino IDE**.
2. Instale as bibliotecas necessárias pelo Gerenciador de Bibliotecas:
   - `LiquidCrystal I2C`
   - `Servo` (já incluída por padrão na IDE)
   - `Wire` (já incluída por padrão na IDE)
3. Monte o circuito conforme o **mapeamento de pinos** acima.
4. Abra o arquivo `.ino` na Arduino IDE.
5. Selecione a placa **Arduino UNO** e a porta correta.
6. Faça o upload do código para a placa.
7. (Opcional) Abra o **Monitor Serial** em **9600 baud** para acompanhar as mensagens de reset/confirmação.

---

## 🔗 Link da simulação

**Tinkercad:** https://www.tinkercad.com/things/kXfMvsFNvE6-global-solution?sharecode=mVWvk2mdvNNYouT6iDLFktNt8nI_f-u9VCuGWuhKyuw

---

## 👥 Integrantes do grupo

| Nome completo | RM |
|---|---|
| Bruno Guilherme Gonçalves de Oliveira | RM573697 |
| Gabriel Cardoso de Sá Finzetto | RM571846 |
| Gabriel Luna Maia | RM570982 |
| João Lucas Magordo Rodrigues | RM572419 |
| Murilo Vieira dos Reis | RM573764 |

---

<div align="center">

**FIAP — Global Solution 2026 · Space Connect**
Projeto ChuvaViva 🌧️🛰️

</div>
