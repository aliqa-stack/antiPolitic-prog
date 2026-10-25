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
* Tidak menggunakan library machine learning eksternal.

## Struktur Repository

```text
naivebayescpp-version/
├── main.cpp
├── README.md
├── LICENSE
└── dataset/
    ├── train/
    │   ├── politicspeech.csv
    │   └── nonpoliticspeech.csv
    └── testing/
        └── test.csv
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
g++ main.cpp -o naivebayes.exe
```

**Linux (GCC):**

```bash
g++ main.cpp -o naivebayes
```

**macOS (Clang):**

```bash
clang++ main.cpp -o naivebayes
```

### 4. Jalankan Program

**Windows — PowerShell:**

```powershell
.\naivebayes.exe
```

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

Dataset menggunakan format CSV dengan dua kolom, yaitu `text` dan `label`.

Contoh:

```csv
text,label
"Contoh teks pidato politik.",politicspeech
"Contoh teks pidato nonpolitik.",nonpoliticspeech
```

File dataset training:

* `dataset/train/politicspeech.csv`
* `dataset/train/nonpoliticspeech.csv`

File dataset testing:

* `dataset/testing/test.csv`

> **Penting:** Dataset yang digunakan saat ini merupakan data dummy untuk keperluan pengembangan dan pengujian awal. Dataset tersebut bukan real dataset yang telah dikurasi atau divalidasi secara sistematis.

Format CSV harus sesuai dengan implementasi parser di dalam program, terutama dalam penanganan koma, tanda kutip, dan baris baru.

## Algoritma

Program menggunakan algoritma Naive Bayes Classifier, yaitu metode klasifikasi probabilistik berdasarkan Teorema Bayes dengan asumsi independensi bersyarat antarfitur.

Tahapan pemrosesan:

1. Membaca dataset training.
2. Melakukan tokenisasi terhadap teks.
3. Menghitung frekuensi kata dan probabilitas masing-masing kelas.
4. Menggunakan Laplace smoothing untuk menangani probabilitas nol pada kata yang tidak ditemukan dalam suatu kelas.
5. Menghitung skor log probabilitas teks masukan untuk setiap kelas.
6. Menentukan hasil klasifikasi berdasarkan skor log probabilitas yang lebih tinggi.

## Hasil Uji Coba Sementara

Pengujian awal dilakukan menggunakan implementasi Naive Bayes Classifier dengan dataset dummy.

Hasil berikut merupakan keluaran dari salah satu percobaan, bukan hasil evaluasi menyeluruh terhadap performa model.

### 1. Hasil Klasifikasi

| Informasi                        | Hasil                |
| -------------------------------- | -------------------- |
| Jumlah kosakata                  | 2.092                |
| Prediksi kelas                   | `non politic speech` |
| Skor log probabilitas politik    | -41594.6             |
| Skor log probabilitas nonpolitik | -40982.7             |

Model memilih kelas `non politic speech` karena skor log probabilitasnya lebih tinggi daripada skor kelas `politic speech`.

Nilai log probabilitas yang negatif merupakan hal yang wajar. Skor ini bukan persentase keyakinan model, sehingga tidak dapat langsung ditafsirkan sebagai probabilitas dalam bentuk persen.

### 2. Statistik Frekuensi Kata

Program juga menghitung kata dengan frekuensi kemunculan tertinggi dan terendah pada setiap sumber data.

| Sumber data         | Kata terbanyak | Frekuensi | Kata paling sedikit | Frekuensi |
| ------------------- | -------------- | --------: | ------------------- | --------: |
| Training politik    | `dan`          |       134 | `1945`              |         1 |
| Training nonpolitik | `dan`          |       147 | `Air`               |         1 |
| Testing             | `yang`         |       229 | `Ajak`              |         1 |

Statistik tersebut menggambarkan frekuensi kata berdasarkan hasil penghitungan program pada dataset yang digunakan.

Kata dengan frekuensi tertinggi atau terendah tidak secara otomatis menentukan hasil klasifikasi. Prediksi tetap didasarkan pada perhitungan probabilitas model.

### 3. Log Proses Klasifikasi

Pada pengujian, program juga menghasilkan skor log probabilitas secara bertahap untuk setiap kata yang diproses.

Contoh keluaran:

```text
Iterasi 5784 - kata: dan
Probabilitas log pidato politik: -41572.5
Probabilitas log pidato non-politik: -40959.3

Probabilitas log pidato politik: -41594.6
Probabilitas log pidato non-politik: -40982.7

Jumlah kosakata: 2092
Hasil Klasifikasi: non politic speech
```

Skor kumulatif berubah seiring pemrosesan kata dalam teks. Untuk menentukan kelas prediksi, model membandingkan skor akhir kedua kelas.

### 4. Interpretasi Hasil

Berdasarkan percobaan tersebut, implementasi berhasil menghasilkan prediksi kelas dan statistik frekuensi kata.

Namun, hasil ini belum cukup untuk menyimpulkan bahwa model memiliki tingkat akurasi tertentu karena:

* Dataset masih berupa data dummy.
* Hasil tersebut hanya berasal dari percobaan yang ditampilkan.
* Belum tersedia pengukuran performa menyeluruh menggunakan label aktual dan metrik evaluasi.
* Frekuensi kata saja tidak menunjukkan kemampuan model dalam mengklasifikasikan teks yang belum pernah dilihat.

Dengan demikian, hasil uji coba ini hanya menunjukkan bahwa alur klasifikasi dasar telah berjalan pada data yang digunakan, bukan membuktikan bahwa model sudah akurat untuk data nyata.

> **Catatan:** Statistik frekuensi kata pada bagian ini mengikuti keluaran program yang diberikan. Hasilnya dapat berubah apabila isi dataset atau implementasi pemrosesan teks diubah.

## Batasan

* Dataset masih berupa data dummy, bukan real dataset.
* Hasil klasifikasi belum mencerminkan performa model pada data dunia nyata.
* Tokenisasi masih sederhana dan belum menangani seluruh variasi bahasa Indonesia.
* Model dapat salah mengklasifikasikan teks yang memiliki karakteristik serupa pada kedua kelas.
* Belum tersedia evaluasi menyeluruh menggunakan metrik accuracy, precision, recall, dan F1-score.
* Definisi kelas politik dan nonpolitik perlu ditetapkan secara konsisten.

## Pengembangan Selanjutnya

Beberapa pengembangan yang dapat dilakukan:

* Mengumpulkan dan memvalidasi dataset nyata yang representatif.
* Memperbaiki tokenisasi dan normalisasi teks bahasa Indonesia.
* Menambahkan evaluasi otomatis menggunakan label aktual pada dataset testing.
* Menghitung accuracy, precision, recall, dan F1-score.
* Menambahkan confusion matrix untuk menganalisis hasil klasifikasi.
* Mengoptimalkan pemrosesan teks dan tampilan output program.

## Tujuan Proyek

Repository ini dibuat untuk pembelajaran, eksperimen, dan eksplorasi implementasi algoritma machine learning menggunakan C++ Standard Library.

Proyek ini bukan sistem produksi dan tidak dimaksudkan untuk menentukan secara mutlak apakah suatu pidato bersifat politik atau nonpolitik.

## Lisensi

Lihat file `LICENSE` untuk informasi mengenai lisensi proyek ini.
