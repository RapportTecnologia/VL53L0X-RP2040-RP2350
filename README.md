# RP2040 VL53L0X Distance Sensor Project

## Como obter este módulo (via workspace principal)

Para garantir a estrutura de diretórios e dependências (ex.: `../I2C`, testes, mocks) conforme o monorepo, recomenda-se clonar o repositório raiz com submódulos:

```bash
git clone --recurse-submodules https://github.com/RapportTecnologia/Sensores_e_Atuadores_do_Kit_BitDogLab.git Sensores_e_Atuadores_do_KIT
cd Sensores_e_Atuadores_do_KIT
git submodule update --init --recursive
```

Atualizar submódulos (quando aplicável):

```bash
git submodule update --remote --merge --recursive
```

Este projeto implementa o firware para o sensor de distância VL53L0X usando o microcontrolador RP2040 (Raspberry Pi Pico ou Pico W) com bibliotecas de alto nível.

## Características

- **Sensor**: VL53L0X Time-of-Flight (ToF) laser ranging sensor
- **Microcontrolador**: RP2040 (Raspberry Pi Pico W)
- **Interface**: I2C (400kHz)
- **Linguagem**: C++ com compatibilidade Arduino
- **SDK**: Raspberry Pi Pico SDK 2.1.0

## Estrutura do Projeto

Nota: Este projeto pode ser utilizado como submódulo dentro de um repositório raiz. O módulo `I2C/` foi movido para o diretório raiz do workspace e é referenciado via `../I2C` no `CMakeLists.txt`.

```
Workspace/
├── I2C/                  # Biblioteca I2C para RP2040
    ├── I2C.h
    └── I2C.cpp   
├── VL53L0X-RP2040-RP2350/
    ├── rp2040-vl5310x.cpp     # Implementação principal
    ├── test/                 # Testes unitários
    ├── CMakeLists.txt        # Configuração de build (usa ../I2C)
    └── build/                # Arquivos de build (gerados)
    ├── vl53l0x/              # Biblioteca VL53L0X original
    │   ├── VL53L0X.h
    │   └── VL53L0X.cpp
```

## Pinagem

| Função | GPIO | Pino Físico |
|--------|------|-------------|
| SDA    | GP4  | Pin 6       |
| SCL    | GP5  | Pin 7       |
| VCC    | 3V3  | Pin 36      |
| GND    | GND  | Pin 38      |

## Bibliotecas Utilizadas

### 1. I2C (Criada para este projeto)
- Interface e funções de apoio para I2C 
- Implementação otimizada para Pico SDK
- Suporte a operações master I2C
- Buffer de 128 bytes para TX/RX

**Principais métodos:**
```cpp
I2C.begin()                    // Inicializa I2C
I2C.setClock(400000)          // Define frequência
I2C.beginTransmission(addr)   // Inicia transmissão
I2C.write(data)               // Escreve dados
I2C.endTransmission()         // Finaliza transmissão
I2C.requestFrom(addr, len)    // Solicita dados
I2C.read()                    // Lê dados recebidos
```

### 2. VL53L0X Library
- Biblioteca completa para sensor VL53L0X
- Suporte a todas as funcionalidades do sensor
- Configuração avançada de parâmetros

**Principais métodos:**
```cpp
sensor.init()                           // Inicializa sensor
sensor.readRangeSingleMillimeters()     // Lê distância (mm)
sensor.setMeasurementTimingBudget(us)   // Define precisão
sensor.setSignalRateLimit(rate)         // Define alcance
sensor.setTimeout(ms)                   // Define timeout
sensor.timeoutOccurred()                // Verifica timeout
```

## Funcionalidades Implementadas

### Configurações do Sensor
- **Timing Budget**: 50ms (configurável) para melhor precisão
- **Signal Rate Limit**: 0.1 MCPS para maior alcance
- **Timeout**: 500ms para operações I2C
- **Frequência I2C**: 400kHz para comunicação rápida

### Recursos Avançados
- Detecção automática do sensor
- Verificação de timeout em operações
- Configuração otimizada para precisão vs velocidade
- Tratamento de erros robusto

## Como Compilar

### Pré-requisitos
- Raspberry Pi Pico SDK 2.1.0
- CMake 3.13+
- Compilador ARM GCC

### Passos de Compilação

1. **Preparar ambiente**:
```bash
cd VL53L0X-RP2040-RP2350
mkdir -p build
cd build
```

2. **Configurar CMake**:
```bash
cmake ..
```

3. **Compilar**:
```bash
cmake --build . --parallel 4
```

4. **Resultado**: O arquivo `rp2040_vl53l0x.uf2` será gerado na pasta `build/`

## Como Usar

### Upload do Firmware
1. Conecte o Pico ao computador segurando o botão BOOTSEL
2. Copie o arquivo `rp2040_vl53l0x.uf2` para o drive RPI-RP2
3. O Pico reiniciará automaticamente

### Monitoramento Serial
- Conecte via USB
- Configure terminal serial (115200 baud)
- Observe as leituras de distância em tempo real

### Exemplo de Saída
```
Waiting for USB connection...
USB connected!
VL53L0X Distance Sensor Example (Using Libraries)
Initializing VL53L0X sensor...
VL53L0X sensor initialized successfully!
Sensor configuration complete. Starting measurements...
Range readings (mm):
Range: 245 mm
Range: 247 mm
Range: 250 mm
```

## Especificações Técnicas

### Sensor VL53L0X
- **Alcance**: 30mm a 2000mm
- **Precisão**: ±3% (típica)
- **Tempo de medição**: 20ms a 300ms (configurável)
- **Interface**: I2C (endereço padrão: 0x29)
- **Alimentação**: 2.8V a 3.3V

### Performance
- **Taxa de amostragem**: ~2 Hz (com configurações atuais)
- **Consumo**: ~19mA durante medição
- **Resolução**: 1mm

## Troubleshooting

### Problemas Comuns

1. **Sensor não detectado**:
   - Verifique conexões I2C
   - Confirme alimentação 3.3V
   - Teste continuidade dos fios

2. **Leituras inconsistentes**:
   - Verifique se há interferência luminosa
   - Ajuste timing budget
   - Verifique superfície do alvo

3. **Timeouts frequentes**:
   - Aumente valor de timeout
   - Verifique qualidade das conexões
   - Reduza frequência I2C se necessário

### Debug
- Use `printf()` para debug adicional
- Monitor serial mostra status detalhado
- LEDs do Pico indicam atividade

## Melhorias Futuras

- [ ] Suporte a múltiplos sensores
- [ ] Interface web para configuração
- [ ] Logging de dados em flash
- [ ] Modo de baixo consumo
- [ ] Calibração automática

## Documentação

Para gerar documentação local deste módulo (se houver `Doxyfile` configurado neste ou no monorepo), execute:

```bash
doxygen Doxyfile
```

A saída HTML ficará disponível em `docs/html/index.html` (ou conforme configuração).

## Licença

Este projeto utiliza bibliotecas com diferentes licenças:
- VL53L0X Library: Baseada em código ST Microelectronics
- I2C: Implementação própria para este projeto
- Pico SDK: BSD 3-Clause License

## Autores e Colaboradores

- Carlos Delfino — consultoria@carlosdelfino.eti.br — GitHub: [@carlosdelfino](https://github.com/carlosdelfino)

Projeto refatorado para usar bibliotecas de alto nível, mantendo compatibilidade com Arduino e otimização para Pico SDK.
