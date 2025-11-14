# Smith–Waterman Algorithm in OpenMP
A highly efficient implementation of the Smith–Waterman local alignment algorithm with both serial and parallel (OpenMP) versions.  
This project demonstrates correctness, diagonal wavefront parallelization, and performance speedup using multiple threads.

---

## 📌 Features
- Serial implementation of Smith–Waterman  
- Parallel OpenMP implementation using wavefront (diagonal) parallelization  
- Debug mode to print the Similarity Matrix & Predecessor Matrix  
- Performance benchmark with multiple thread counts  
- Clean and minimal C implementation

---

## 📊 Sample Output

### **Serial Output**
![Serial Output](https://github.com/Kartikay77/PARALLELIZATION-USING-WATERMAN-SMITH-ALGORITHM/blob/main/Serial.PNG)

### **Parallel Output**
![Parallel Output](https://github.com/Kartikay77/PARALLELIZATION-USING-WATERMAN-SMITH-ALGORITHM/blob/main/Paraller.png)

### **OpenMP Timing Comparison**
![Time Comparison](https://github.com/Kartikay77/PARALLELIZATION-USING-WATERMAN-SMITH-ALGORITHM/blob/main/Time_OMP.png)

---

## 🚀 How to Use

### 📌 Serial Version  
[serial_smithW.c](https://github.com/Kartikay77/PARALLELIZATION-USING-WATERMAN-SMITH-ALGORITHM/blob/main/serial_smithW.c)

#### Compile:
\`\`\`bash
gcc serial_smithW.c -o serial_smithW -fopenmp -DDEBUG
\`\`\`

#### Run:
\`\`\`bash
./serial_smithW <num_columns> <num_rows>
\`\`\`

---

### ⚡ Parallel OpenMP Version  
[omp_smithW.c](https://github.com/Kartikay77/PARALLELIZATION-USING-WATERMAN-SMITH-ALGORITHM/blob/main/omp_smithW.c)

#### Compile:
\`\`\`bash
gcc omp_smithW.c -o omp_smithW -fopenmp -DDEBUG
\`\`\`

#### Run:
\`\`\`bash
./omp_smithW <num_threads> <num_columns> <num_rows>
\`\`\`

---

## 📈 Parallelization Strategy  
This implementation uses **anti-diagonal (wavefront) parallelism**, allowing all cells on the same diagonal to be computed simultaneously because they are independent.

This approach provides significant speedup over the serial version, as shown in the timing comparison.

---

## 🐞 Bug Reports & Feature Requests  
Report issues or suggest features:  
👉 https://github.com/Kartikay77/PARALLELIZATION-USING-WATERMAN-SMITH-ALGORITHM/issues

---

## 📌 To-Do  
- Improve visualization of alignment path  
- Add more extensive speedup benchmarking  
- Add documentation of wavefront parallelism  

---

## 🤝 Contributing
1. Fork the repository  
2. Create a feature branch  
3. Test your changes  
4. Submit a pull request  

---

## 👤 Author  
Developed by **[Kartikay Gupta](https://github.com/Kartikay77)**

