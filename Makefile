FROG_EXE = frog
export CXX

$(FROG_EXE): frogs.cpp
	$(CXX) -g $< -o $(FROG_EXE) -std=c++0x  
  		
clean:
	rm -f $(FROG_EXE)	\
	rm -rf logs
	

