# 💻💻 MODEL MACHINE LEARNING YANG PEMERINTAH GAMAU LU TAU 💀

Implementasi sederhana algoritma **Naive Bayes Classifier menggunakan C++ Standard Library**, tanpa library machine learning tambahan.

Proyek ini merupakan eksperimen klasifikasi teks untuk membedakan dua kategori pidato:

* `politic speech`
* `non politic speech`

> **Status proyek:** Masih dalam tahap pengembangan. Dataset yang digunakan saat ini merupakan data dummy, bukan real dataset. Hasil klasifikasi belum dapat dijadikan tolok ukur performa model pada data dunia nyata.

## Fitur

* Implementasi algoritma Naive Bayes Classifier menggunakan C++.
* Klasifikasi teks ke dalam kategori politik dan nonpolitik.
* Perhitungan probabilitas menggunakan Laplace smoothing.
* Tokenisasi teks sederhana.
* Pemisahan dataset training dan testing.
* Statistik frekuensi kata dari dataset politik, nonpolitik, dan testing.
* Penyimpanan model binary berversi, termasuk statistik kelas, smoothing, dan konfigurasi preprocessing.
* Evaluasi model tersimpan melalui opsi `--load-model-evaluation`.
* Tidak menggunakan library machine learning eksternal.

## Struktur Repository

```text
naivebayescpp-version/
├── main.cpp
├── include/
│   ├── modelpersistence.h
│   └── ModelEvaluation.h
├── src/
│   ├── modelpersistence.cpp
│   └── ModelEvaluation.cpp
├── README.md
├── LICENSE
└── dataset/
    ├── train/
    │   ├── politicspeech/
    │   │   └── *.csv
    │   └── nonpoliticspeech/
    │       └── *.csv
    └── testing/
        ├── test-1n.csv
        ├── test-2n.csv
        ├── test-3p.csv
        ├── test-4n.csv
        └── test-5p.csv
```

## Persyaratan

Sebelum menjalankan program, pastikan kamu memiliki:

* Compiler C++ seperti GCC atau Clang.
* Terminal untuk menjalankan perintah.
* Dataset yang tersedia di direktori `dataset/`.

## Cara Menjalankan

### 1. Clone Repository

Ganti `<URL-REPOSITORY>` dengan URL repository Git yang sebenarnya.

```bash
git clone <URL-REPOSITORY>
```

### 2. Masuk ke Direktori Repository

Ganti `<NAMA-FOLDER>` dengan nama direktori hasil clone.

```bash
cd <NAMA-FOLDER>
```

### 3. Compile Program

**Windows (GCC/MinGW-w64):**

```powershell
g++ -Iinclude main.cpp src/* -o naivebayes.exe
```

**Linux (GCC):**

```bash
g++ -Iinclude main.cpp src/* -o naivebayes
```

**macOS (Clang):**

```bash
clang++ -Iinclude main.cpp src/* -o naivebayes
```

### 4. Jalankan Program

**Windows — PowerShell:**

```powershell
.\naivebayes.exe
```

Secara default program melatih model dari dataset, menyimpannya ke folder
`models/` dengan nama `models-naivebayes-smoothingAlpha.bin`, lalu mengevaluasi
hasilnya. Folder `models/` dibuat otomatis jika belum ada.

Untuk evaluasi saja menggunakan model yang sudah tersimpan, tanpa memuat ulang
dataset training:

```powershell
.\naivebayes.exe --load-model-evaluation
```

Mode tersebut hanya memuat file model dan mengevaluasi file `.csv` dalam
`dataset/testing/`. File testing harus berakhiran `p` (politik) atau `n`
(nonpolitik) sebelum ekstensi. Program menampilkan hasil tiap file, akurasi,
precision, recall, dan F1-score agregat untuk keseluruhan model (micro average),
bukan metrik terpisah untuk setiap kelas.

File `models/models-naivebayes-smoothingAlpha.bin` menyimpan jumlah dokumen per
kelas, frekuensi kata per kelas, total kata, kosakata, nilai Laplace smoothing,
konfigurasi preprocessing, dan versi format binary. Jika format file tidak
didukung atau datanya rusak, program menampilkan error dan berhenti alih-alih
memakai model yang tidak valid.

**Linux dan macOS:**

```bash
./naivebayes
```

**Catatan:**

* Jalankan program dari direktori utama repository agar path dataset relatif dapat ditemukan.
* Jika hanya mengubah isi file CSV, kamu tidak perlu melakukan kompilasi ulang.
* Jika mengubah kode C++, lakukan kompilasi ulang sebelum menjalankan program.
* Nama file hasil kompilasi bergantung pada perintah yang digunakan.

## Format Dataset

Dataset training menggunakan format satu kolom, yaitu `text`. Kelas ditentukan
oleh folder tempat file disimpan.

Contoh:

```csv
text
"Contoh teks pidato."
```

File dataset training:

* politicspeech : 
 - `dataset/train/politicspeech/1.csv`
 - `dataset/train/politicspeech/2.csv`
 - `dataset/train/politicspeech/3.csv`
 - `dataset/train/politicspeech/4.csv`
 - `dataset/train/politicspeech/5.csv`
 - `dataset/train/politicspeech/6.csv`
 
* nonpoliticspeech: 
 - `dataset/train/nonpoliticspeech/1.csv`
 - `dataset/train/nonpoliticspeech/2.csv`
 - `dataset/train/nonpoliticspeech/3.csv`
 - `dataset/train/nonpoliticspeech/4.csv`
 - `dataset/train/nonpoliticspeech/5.csv`
 - `dataset/train/nonpoliticspeech/6.csv`

Setiap file training menggunakan header `text`, kemudian berisi teks pidato
untuk kelas sesuai nama foldernya. File di folder `politicspeech/` masuk kelas
politik, sedangkan file di folder `nonpoliticspeech/` masuk kelas nonpolitik.
Program memuat semua file `.csv` yang ada di masing-masing folder.

File dataset testing:

* `dataset/testing/test-1n.csv`
* `dataset/testing/test-2n.csv`
* `dataset/testing/test-3p.csv`
* `dataset/testing/test-4n.csv`
* `dataset/testing/test-5p.csv`

File testing berisi teks pidato. Label aktual dibaca dari nama file: akhiran `p`
berarti politik dan akhiran `n` berarti nonpolitik. Program mengklasifikasikan
setiap file, membandingkan label aktual dengan tebakan model, lalu menampilkan
jumlah prediksi benar dan akurasi testing.

## Algoritma

Program menggunakan algoritma Naive Bayes Classifier, yaitu metode klasifikasi probabilistik berdasarkan Teorema Bayes dengan asumsi independensi bersyarat antarfitur.

Tahapan pemrosesan:

1. Membaca dataset training.
2. Melakukan tokenisasi terhadap teks.
3. Menghitung frekuensi kata dan probabilitas masing-masing kelas.
4. Menggunakan Laplace smoothing untuk menangani probabilitas nol pada kata yang tidak ditemukan dalam suatu kelas.
5. Menghitung skor log probabilitas teks masukan untuk setiap kelas.
6. Menentukan hasil klasifikasi berdasarkan skor log probabilitas yang lebih tinggi.

## Hasil Pengujian

Berikut catatan hasil dari dataset dan output program yang digunakan.

### Statistik model

| Informasi | Hasil |
| --- | ---: |
| Jumlah kosakata | 3.074 |
| Kata terbanyak di training politik | `yang` (308 kali) |
| Kata paling sedikit di training politik | `0,0007` (1 kali) |
| Kata terbanyak di training nonpolitik | `yang` (95 kali) |
| Kata paling sedikit di training nonpolitik | `Antariksa` (1 kali) |

Skor log yang tercetak sebelum rincian file testing:

| Kelas | Skor log probabilitas |
| --- | ---: |
| Politik | -44.404,7 |
| Nonpolitik | -42.259,6 |

### Perbandingan hasil testing

Label aktual diambil dari nama file (`p` = politik, `n` = nonpolitik). Skor
log probabilitas lebih tinggi (lebih dekat ke nol) menjadi tebakan model.

| File testing | Label aktual | Skor politik | Skor nonpolitik | Tebakan model | Hasil |
| --- | :---: | ---: | ---: | :---: | --- |
| `test-1n.csv` | n | -49.472,9 | -46.986,1 | n | Benar |
| `test-2n.csv` | n | -46.707,5 | -45.750,4 | n | Benar |
| `test-3p.csv` | p | -52.215,5 | -49.855,9 | n | Salah |
| `test-4n.csv` | n | -48.235,3 | -47.609,4 | n | Benar |
| `test-5p.csv` | p | Tidak dicatat | Tidak dicatat | n | Salah |

**Akurasi testing: 3/5 (60%).**

### Hasil evaluasi keseluruhan

| Metrik | Hasil |
| --- | ---: |
| Akurasi testing | 3/5 (60%) |
| Prediksi benar keseluruhan | 3 |
| Prediksi salah keseluruhan | 2 |
| Precision keseluruhan (micro) | 60% |
| Recall keseluruhan (micro) | 60% |
| F1-score keseluruhan (micro) | 60% |

Statistik kata pada file testing:

| File testing | Kata terbanyak | Frekuensi | Kata paling sedikit | Frekuensi |
| --- | --- | ---: | --- | ---: |
| `test-1n.csv` | `yang` | 229 | `Ajak` | 1 |
| `test-2n.csv` | `yang` | 292 | `Ajak` | 3 |
| `test-3p.csv` | `yang` | 278 | `1945` | 3 |
| `test-4n.csv` | `yang` | 321 | `30` | 3 |
| `test-5p.csv` | `dan` | 229 | `1982` | 3 |

Output yang dicatat tidak memuat skor log probabilitas untuk `test-5p.csv`.
Hasil ini adalah catatan dari lima file testing tersebut, bukan jaminan akurasi
model pada dataset lain.

## Batasan

* 80% Dataset masih berupa data dummy, bukan real dataset.
* Hasil klasifikasi belum mencerminkan performa model pada data dunia nyata.
* Tokenisasi masih sederhana dan belum menangani seluruh variasi bahasa Indonesia.
* Model dapat salah mengklasifikasikan teks yang memiliki karakteristik serupa pada kedua kelas.
* Belum tersedia evaluasi menyeluruh menggunakan metrik accuracy, precision, recall, dan F1-score.
* Definisi kelas politik dan nonpolitik perlu ditetapkan secara konsisten.

## Tujuan Proyek

Repository ini dibuat untuk pembelajaran, eksperimen, dan eksplorasi implementasi algoritma machine learning menggunakan C++ Standard Library.

Proyek ini bukan sistem produksi dan tidak dimaksudkan untuk menentukan secara mutlak apakah suatu pidato bersifat politik atau nonpolitik.

## Lisensi

Lihat file `LICENSE` untuk informasi mengenai lisensi proyek ini.

* File untuk dataset training pidatospeech diambil dari setkab.go.id

## Notes

* Kami telah mengganti dataset training dengan dataset nyata. Tetapi dataset tersebut tidak balance dan menyebabkan models overfithing, Kedepannya kami akan mengembangkan model ini dan memperbanyak dataset.
