all: compile print_usage

compile:
	nvcc triliteration.cu -o triliteration
	
print_usage:
	@echo "Usage: ./triliteration <number_of_input_in_power_2> <number_of_blocks> <number_of_threads> <input_file> <check_file>"
	
clean:
	rm triliteration
