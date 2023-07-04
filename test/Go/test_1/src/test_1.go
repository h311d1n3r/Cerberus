package main

import (
	"github.com/fatih/color"
	"bytes"
	"crypto/aes"
	"crypto/cipher"
	"crypto/rand"
	"encoding/base64"
	"fmt"
	"io"
)

func main() {
	fmt.Println("Hey ! Starting the program...")

	key := []byte("The secret key !")

	plaintext := []byte("Message to encrypt")

	iv := make([]byte, aes.BlockSize)
	if _, err := io.ReadFull(rand.Reader, iv); err != nil {
		panic(err)
	}

	block, err := aes.NewCipher(key)
	if err != nil {
		panic(err)
	}

	mode := cipher.NewCBCEncrypter(block, iv)

	plaintext = padPKCS7(plaintext, aes.BlockSize)

	ciphertext := make([]byte, len(plaintext))

	mode.CryptBlocks(ciphertext, plaintext)

	ciphertext = append(iv, ciphertext...)

	encodedText := base64.StdEncoding.EncodeToString(ciphertext)
	fmt.Println("Encrypted text (base64):", encodedText)

	decodedText, err := base64.StdEncoding.DecodeString(encodedText)
	if err != nil {
		panic(err)
	}

	iv = decodedText[:aes.BlockSize]
	ciphertext = decodedText[aes.BlockSize:]

	mode = cipher.NewCBCDecrypter(block, iv)

	decryptedText := make([]byte, len(ciphertext))

	mode.CryptBlocks(decryptedText, ciphertext)

	decryptedText = unpadPKCS7(decryptedText)

	fmt.Println("Decrypted text:", string(decryptedText))
	
	color.Cyan("Prints text in cyan.")
}

func padPKCS7(data []byte, blockSize int) []byte {
	padding := blockSize - (len(data) % blockSize)
	padText := bytes.Repeat([]byte{byte(padding)}, padding)
	return append(data, padText...)
}

func unpadPKCS7(data []byte) []byte {
	length := len(data)
	unpadding := int(data[length-1])
	return data[:length-unpadding]
}
