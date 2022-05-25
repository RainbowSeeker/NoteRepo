#变量操作
objects = main.o 
objects += main.o 

name = 123
#类似于指针，curname = name最终值
curname = $(name)
#curname = name当前值
curname := $(name)
#若curname没有值则=name
curname ?= $(name)
name = 456
print:
	@echo curname: $(curname)

main: $(objects)
	gcc -o main $(objects)

#声明伪目标
.PHONY:clean
clean:
	rm main *.o

# $< 依赖文件的第一个，如果通配符则为文件的集合
%.o: %.c
	gcc -c $<

	