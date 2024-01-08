package main

import (
	"fmt"
	"math/rand"
	"os"
	"os/user"
	"time"
)

const (
	LINE = 20
	CAP  = 25
)

const DATA_DIR = "document/rngst_data"

func main() {
	user, err := user.Current()
	if err != nil {
		fmt.Println(err)
		os.Exit(-1)
	}
	dataDir := user.HomeDir + "/" + DATA_DIR
	if _, err = os.Stat(dataDir); err != nil {
		if err = os.Mkdir(dataDir, 0775); err != nil {
			fmt.Println(err)
			os.Exit(-1)
		}
	}
	now := time.Now().Format("2006-01-02.txt")
	filePath := dataDir + "/" + now
	_, err = os.Stat(filePath)
	if err != nil {
		f, err := os.Create(filePath)
		if err != nil {
			fmt.Println(err)
			os.Exit(-1)
		}
		defer f.Close()
		matrix := newMatrix(LINE, CAP)
		_, err = f.Write([]byte(matrixToString(matrix)))
		if err != nil {
			fmt.Println(err)
			os.Exit(-1)
		}
		showMatrix(matrix)
	} else {
		f, err := os.Open(filePath)
		if err != nil {
			fmt.Println(err)
			os.Exit(-1)
		}
		buf := make([]byte, 1024)
		f.Read(buf)
		fmt.Println(string(buf))
	}

}

func newMatrix(n, m int) [][]int {
	matrix := make([][]int, n)
	for i := range matrix {
		matrix[i] = make([]int, m)
		for j := 0; j < m; j++ {
			matrix[i][j] = rand.Intn(99) + 1
		}
	}
	return matrix
}

func showMatrix(matrix [][]int) {
	for i := range matrix {
		for j := range matrix[i] {
			fmt.Printf("%02d ", matrix[i][j])
		}
		fmt.Println()
	}
}

func matrixToString(matrix [][]int) string {
	str := ""
	for i := range matrix {
		line := ""
		for j := range matrix[i] {
			line += fmt.Sprintf("%02d ", matrix[i][j])
		}
		line += "\n"
		str += line
	}
	return str
}
