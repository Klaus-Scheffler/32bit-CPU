int main() {
	int x[5];
	int i = 0;
	int *ptr = 1;
	
	while (i<5) {
	ptr = x+i;
	*ptr = i;
	i = i + 1;
}
}
