ifeq ($(shell uname), Darwin)
	CPLEXDIR  = /Applications/ILOG/CPLEX_Studio126/cplex
	CONCERTDIR = /Applications/ILOG/CPLEX_Studio126/concert
	SYSTEM = x86-64_osx
	BCFLAGS = -std=c++11 -stdlib=libstdc++
else
	CPLEXDIR  = /usr/ilog/cplex
	CONCERTDIR = /usr/ilog/concert
	SYSTEM = x86-64_sles10_4.1
	BCFLAGS = -std=c++11
endif

BITS_OPTION = -m64
LIBFORMAT   = static_pic

#### Diretorios com as libs do cplex ####
   
CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)

#### define o compilador ####
CPPC = g++
#############################

#### opcoes de compilacao e includes ####
CCOPT = $(BITS_OPTION) -Wall -O3 -g -fPIC -fexceptions -DNDEBUG -DIL_STD -std=c++11 -fpermissive 
CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include
CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR)
#############################

#### flags do linker ####
CCLNFLAGS = -L$(CPLEXLIBDIR) -lilocplex -lcplex -L$(CONCERTLIBDIR) -lconcert -lm -lpthread 
#############################

#### diretorios com os source files e com os objs files ####
SRCDIR = src
OBJDIR = obj
#############################

#### lista de todos os srcs e todos os objs ####
SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCS))
#############################

#### regra principal, gera o executavel ####
bc: $(OBJS) 
	@echo  "\033[31m \nLinking all objects files: \033[0m"
	$(CPPC) $(BCFLAGS) $(BITS_OPTION) $(OBJS) -o $@ $(CCLNFLAGS)
############################

#inclui os arquivos de dependencias
-include $(OBJS:.o=.d)

#regra para cada arquivo objeto: compila e gera o arquivo de dependencias do arquivo objeto
#cada arquivo objeto depende do .c e dos headers (informacao dos header esta no arquivo de dependencias gerado pelo compiler)
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@echo  "\033[31m \nCompiling $<: \033[0m"
	$(CPPC) $(CCFLAGS) -c $< -o $@
	@echo  "\033[32m \ncreating $< dependency file: \033[0m"
	$(CPPC) $(CCFLAGS) -MM $< > $(basename $@).d
	@mv -f $(basename $@).d $(basename $@).d.tmp #proximas tres linhas colocam o diretorio no arquivo de dependencias (g++ nao coloca, surprisingly!)
	@sed -e 's|.*:|$(basename $@).o:|' < $(basename $@).d.tmp > $(basename $@).d
	@rm -f $(basename $@).d.tmp

#delete objetos e arquivos de dependencia
clean:
	@echo "\033[31mcleaning obj directory \033[0m"
	@rm -f $(OBJDIR)/*.o $(OBJDIR)/*.d
	@rm -f bc

rebuild: clean bc

