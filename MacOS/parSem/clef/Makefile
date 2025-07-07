# Compilateur C++
CXX = g++

# Drapeaux de compilation
CXXFLAGS = -DNOUNSIGNEDINT

# Nom de l'exécutable
TARGET = xipkey

# Fichiers source
SRCS = xipkey.cxx

# Fichiers objet
OBJS = $(SRCS:.cxx=.o)

# Règle par défaut
all: $(TARGET)

# Règle pour créer l'exécutable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Règle pour créer les fichiers objet
%.o: %.cxx
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Règle de nettoyage
clean:
	rm -f $(OBJS) $(TARGET)

