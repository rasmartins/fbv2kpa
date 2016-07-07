CXXFLAGS := -Wall -O2

main: Debug.hpp  FbvConstants.hpp  FbvFrame.hpp  Fbv.hpp  FbvParser.hpp  main.cpp  Serial.hpp  SerialLinux.hpp
	$(CXX) $(CXXFLAGS) -o main main.cpp -lpthread

clean:
	$(RM) main *.o
