package main

import (
	"fmt"
	"time"
)

func main() {
	t := time.Tick(1 * time.Second)
	var i = 0
	for range t {
		fmt.Println(i)
		i++
	}
}
