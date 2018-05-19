from dem import dem
import os

bash_filename = 'environment.bash'

def write_environment_bash():
	bash_file = open(bash_filename, 'r')
	lines = []

	lines.append('alias waf='+os.environ["DEPENDENCY_PATH"]+'/waf')
	bash_file.write(lines)

def main():
	dem.get_dem_packages()
	write_environment_bash()