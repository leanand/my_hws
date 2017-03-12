package main

import "fmt"

type reduce_out struct{
  word string
  count int
}

type reduce_struct struct{
  functor ReduceFunc
  accum_str string
  accum_int int
}

type Cmap struct{
  cmap_ar map[string]int
  add_chan chan string
  stop_chan chan bool
  ask_chan chan string
  count_chan chan int
  reduce_chan chan reduce_struct
  reduce_out_chan chan reduce_struct
}

var _cmap Cmap


func NewChannelMap() EmergingMap {
  _cmap.cmap_ar = make(map[string]int)
  _cmap.add_chan = make(chan string, ADD_BUFFER_SIZE)
  _cmap.stop_chan = make(chan bool)
  _cmap.ask_chan = make(chan string, ASK_BUFFER_SIZE)
  _cmap.count_chan = make(chan int, ASK_BUFFER_SIZE)
  _cmap.reduce_chan = make(chan reduce_struct)
  _cmap.reduce_out_chan = make(chan reduce_struct)
  return _cmap
}

func (cmap Cmap) Listen (){
  for {
      select {
        case add_string := <- _cmap.add_chan:
          // fmt.Println("Adding word ", ask_string)
          count, ok := _cmap.cmap_ar[add_string]
          if ok {
            _cmap.cmap_ar[add_string] = count + 1
          }else{  
            _cmap.cmap_ar[add_string] = 1
          }
        case ask_string := <- _cmap.ask_chan:
          count, ok := _cmap.cmap_ar[ask_string]
          if(ok){
            _cmap.count_chan <- count
          }else{
            _cmap.count_chan <- 0
          }
        case _req := <-_cmap.reduce_chan:
          for k,v := range _cmap.cmap_ar {
            _req.accum_str, _req.accum_int = _req.functor(k, v, _req.accum_str, _req.accum_int)
          }
          _cmap.reduce_out_chan <- _req
        case <- _cmap.stop_chan:
          close(_cmap.add_chan)
          close(_cmap.ask_chan)
          close(_cmap.stop_chan)
          fmt.Println("Stopping")
          break
      }
  }  
}

func (cmap Cmap) AddWord (input_string string){
  _cmap.add_chan <- input_string
}


func (cmap Cmap) GetCount (inputString string) int{
  _cmap.ask_chan <- inputString
  return <-_cmap.count_chan
}

func (cmap Cmap) Reduce (functor ReduceFunc, accum_str string, accum_int int) (string, int){
  var _red_struct reduce_struct
  _red_struct.functor = functor
  _red_struct.accum_str = accum_str
  _red_struct.accum_int = accum_int
  _cmap.reduce_chan <- _red_struct
  _red_struct = <-_cmap.reduce_out_chan
  return _red_struct.accum_str, _red_struct.accum_int
}

func (cmap Cmap) Stop (){
  fmt.Println(" Stopping\n");
  _cmap.stop_chan <- true
}