package main

import (
	"os"
	"fmt"
	"log"
	"bufio"
	"strconv"
	"strings"
)

type DataFile struct {
	name, newName string
}

func main() {
	files := []DataFile{
		{
			name: "mono_fess05_step50_samp15",
			newName: "monofessura",
		},
		{
			name: "3_dual_fess01_step50_samp15_best",
			newName: "doppia_fessura",
		},
		{
			name: "2_dual2",
			newName: "doppia_fessura_2",
		},
	}

	for _, datafile := range files {
		file, err := os.Open(fmt.Sprintf("./data/%s.dat", datafile.name))
		if err != nil {
			log.Fatal(err)
		}
		defer file.Close()

		scanner := bufio.NewScanner(file)
		var builder strings.Builder

		for scanner.Scan() {
			line := scanner.Text()
			columns := strings.Split(line, "\t")
			x, err := strconv.ParseFloat(columns[0], 64)
			if err != nil {
				log.Fatal(err)
			}
			x *= 10e-7
			errX, err := strconv.ParseFloat(columns[2], 64)
			if err != nil {
				log.Fatal(err)
			}
			errX *= 10e-7
			builder.WriteString(fmt.Sprintf("%9.9f\t%s\t%9.9f\n", x, columns[1], errX))
		}

		outFile, err := os.Create(fmt.Sprintf("./data/%s.meters.dat", datafile.newName))
		if err != nil {
			log.Fatal(err)
		}
		defer outFile.Close()
		outFile.Write([]byte(builder.String()))
	}
}
