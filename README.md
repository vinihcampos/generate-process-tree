[![license](https://img.shields.io/github/license/mashape/apistatus.svg)](https://opensource.org/licenses/MIT)
## Overview

A simple application to visualize processes' tree of Linux based systems. The main part of code was developed with C++, which catch processes informations on folder <b>/proc</b> and save them in a json file. The graphical interface uses HTML, CSS and Javascript to show the informations as clearly as possible.     

## Installation

To use this application is needed install <b>Boost Filesystem Library</b>: 

<b>Debian Based</b>: `sudo apt-get install libboost-all-dev`

<b>Fedora</b>: `yum install boost-devel`

## Usage

To execute system, first compile the project running the file `./compile.sh`. After that, execute the program `./bin/generate [number_of_process] [time_interval_in_seconds]`. The program will be running on terminal, and now open the file `pages/index.html` with some browser to see the processes' tree. 

## Example

A screen of tree's visualization:

<p align="center"><img src="https://github.com/Vinihcampos/generate-process-tree/blob/master/page/example.jpg" width=70% /></p>

## Team

| [![Vinicius Campos](https://avatars.githubusercontent.com/Vinihcampos?s=100)<br /><sub>Vinicius Campos</sub>](http://lattes.cnpq.br/4806707968253342)<br />[👀](https://github.com/raquel-oliveira/numerical-analysis/commits?author=Vinihcampos)  | [![Vitor Rodrigues Greati](https://avatars.githubusercontent.com/greati?s=100)<br /><sub>Vitor Rodrigues Greati</sub>](http://greati.github.io)<br />[👀](https://github.com/raquel-oliveira/numerical-analysis/commits?author=greati)
| :---: | :---: | :---: | :---: |
 
