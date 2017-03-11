all::sin
sin:  
	g++ -std=c++11 main.cpp image_content.cpp image_translation.cpp error_detect.cpp -o single_cycle

clean:
	rm -f single_cycle

scheck:
	diff snapshot.rpt snapshot_mine.rpt 

echeck:
	diff error_dump.rpt error_dump_mine.rpt 

run:
	./single_cycle

ys:
	diff snapshot.rpt snapshot_mine.rpt -y

ye:
	diff error_dump.rpt error_dump_mine.rpt -y
