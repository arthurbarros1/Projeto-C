# Nome do executável
EXECUTABLE := jogo

# Diretório de build
BUILD_DIR := build

# Opções de compilação
GHC_OPTIONS := -Wall -Werror -odir $(BUILD_DIR) -hidir $(BUILD_DIR)

# Encontrar todos os arquivos .hs na pasta src/
SOURCES := $(wildcard src/*.hs)

# Obter os nomes dos módulos a partir dos arquivos .hs
MODULES := $(SOURCES:src/%.hs=%)

# Regra para compilar o projeto
all: $(EXECUTABLE)

$(EXECUTABLE): $(addprefix $(BUILD_DIR)/, $(MODULES:=.o))
    ghc $(GHC_OPTIONS) -o $@ $^

$(BUILD_DIR)/%.o: src/%.hs
    ghc $(GHC_OPTIONS) -c $<

# Regra para limpar os arquivos de compilação
clean:
    rm -rf $(BUILD_DIR)/* $(EXECUTABLE)

.PHONY: all clean