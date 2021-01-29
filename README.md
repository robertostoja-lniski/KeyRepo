# KeyRepo #
Praca Inżynierska

## Opis repozytorium ##
Jest to projekt realizowany w ramach pracy inzynierskiej. Tworzy repozytorium do przechowywania kluczy prywatnych oraz aplikację rezliującą operacje zabezpieczające dane w oparciu o te klucze. Dokładny jego opis znajduje się w dokumentacji, a poniżej przedstawione są ogólne operacje.

Na repozytorium znajduje się:
* Kod tworzący partycję po stronie jądra systemu (lub jej emulację).
* Aplikacja do działań kryptograficznych wykorzystująca OpenSSL.
* Zestaw ponad 200 testów

## Uruchomienie ##
### Emulacja ###
Jeśli korzysta się z emulacji należy zdefiniowaną stałą EMULATION w pliku KernelEmulation.c oraz KeyRepoSyscallWrapper.h ustawić na 1, dla właściwej partycji EMULATION powinna przyjmować wartość 0.

### Partycja w jądrze Linuksa ###
Aby stworzyć partycję w jądrze Linuksa wymagane jest:
* Ściągnięcie jądra Linuksa (narzędzie było tworzone dla wersji 5.4.80)
```
  wget -P ~/ https://cdn.kernel.org/pub/linux/kernel/v5.x/linux-5.4.80.tar.xz
```
* Utworzenie katalogu src w folderze ze spakowanym jądrem
* Skopiowanie repozytorium do folderu src
* Uruchomienie pliku setup/custom_setup.sh z poziomu folderu ze spakowanym jądrem
* Skompilowanie jądra Linuksa i wgranie go na stacji roboczej. Z katalogu linux-5.4.80 wykonanie kolejno:
  * make menuconfig (wybór domyślnych opcji)
  * make -jN
  * make all -jN (niektóre wersje systemu operacyjnego wymagają tego polecenia do zbudowania modułów)
  * sudo make modules_install -jN
  * sudo make_install
  * reboot
  
Z punktu widzenia programisty nie ma znaczenia czy korzysta z partycji po stronie jądra czy emulacji. Wszystkie konfiguracje jądra (zmiany w plikach Makefile czy dodanie nowych wywołań) są ustawiane automatycznie przez skrypt konfigurujący.

## Kompilacja aplikacji ##
Aplikacja była tworzona w programie CLion, ale posiada standardowy podział na pliki źródłowe i nagłówkowe i jest kompilowana poleceniem cmake.
Ważne jest, żeby ustawić ścieżki do tworzenia emulacji w pliku KernelEmulation.h na lokalne. 

z katalogu głównego projektu należy uruchomić:
```
  cmake --build cmake-build-debug --target KeyRepo
```
lub dla uruchomienia testów
```
  cmake --build cmake-build-debug --target tests
```
W obydwu przypadkach aplikacja zbuduje się w podanym folderze cmake-build-debug/source

## Polecania obsługujące repozytorium ##
Dodano wywołania systemowe do zarzązania kluczami na partycji

```
  long get_key_num(void);
  long write_key(const char __user *key, const size_t keyLen, uint64_t __user *id, int __user uid, int __user gid);
  long read_key(const uint64_t id, char __user * key, uint64_t keyLen, int __user uid, int __user gid);
  long remove_key(const uint64_t __user id, int __user uid, int __user gid);
  long get_mode(const uint64_t __user id, int __user * output, int __user uid, int __user gid);
  long set_mode(const uint64_t __user id, int __user newMode, int __user uid, int __user gid);
  long get_key_size(const uint64_t __user id, uint64_t* size, int __user uid, int __user gid);
```  
## Polecenia aplikacji ##
Aplikacja wspiera następujące polecenia:

```
USAGE:
KEY MANAGEMENT:
	create-key         path_to_private_key_id  path_to_public_key  key_length key_type [overwrite]
	get-private-key    path_to_private_key_id  path_to_file_with_key_value             [overwrite]
	delete-key         path_to_private_key_id  path_to_public_key
KEY OPERATIONS:
	sign               path_to_private_key     path_to_file_to_be_signed path_to_signature       [overwrite]
	check-signature    path_to_public_key      path_to_signed_file       path_to_signature
	encrypt-file       path_to_key             path_to_iv                path_to_file              path_to_encrypted_file  [overwrite]
	decrypt-file       path_to_key             path_to_iv                path_to_encrypted_file    path_to_decrypted_file  [overwrite]
```
