all::sin
sin:  
	g++ -std=c++11 main.cpp image_content.cpp image_translation.cpp error_detect.cpp -o single_cycle

clean:
	rm -f single_cycle
