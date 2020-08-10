'use strict'

console.clear();
/** Get a nested property from an object without returning any errors.
 * If the property or property chain doesn't exist, undefined is returned.
 * Property names with spaces may use either dot or bracket "[]" notation.
 * Note that bracketed property names without surrounding quotes will fail the lookup.
 *      e.g. embedded variables are not supported.
 * @param {Object} obj The object to check
 * @param {string} prop The property or property chain to get (e.g. obj.prop1.prop1a or obj['prop1'].prop2)
 * @returns {*|undefined} The value of the objects property or undefined if the property doesn't exist
 */
function getProp(obj, prop) {
    
    if (typeof obj !== 'object') throw 'getProp: obj is not an object';
    if (typeof prop !== 'string') throw 'getProp: prop is not a string';

    // Replace [] notation with dot notation
    prop = prop.replace(/\[["'`](.*)["'`]\]/g, ".$1");

    return prop.split('.').reduce(function (prev, curr) {
        return prev ? prev[curr] : undefined;
    }, obj || self);
} // --- end of fn getProp() --- //

function hourTohhmmss(hour) {
    var n = new Date(0,0);
    n.setSeconds(hour * 60 * 60);
    return (n.toTimeString().slice(1, 8));
}

Vue.component('apexchart', VueApexCharts)

// Initialise Vue - this is where the grunt-work all happens
var app = new Vue({
    
    el: "#app",
    
    /*
    components: {
        'apexchart': VueApexCharts,
        'VueSvgGauge':VueSvgGauge
    },
    */
    
    // We don't really need a function here but you do in components - keeping things consistent
        
    data: {
        // Which tab should be active?
        tabIndex: 0,
        var1: 0,
        var2: 0,
        var3:0,
        var11:0,
        inputUsername: null,
        inputPassword: null,
        message: null,
        testearray: [],
        rand:[],
        lastFreq:[],
        lastInt:[],
        lastVolt:[],
        value:0,
        seriesFreq: [],
        seriesInt: [],
        seriesVolt: [],
        valorRemoto:0,
        valorManual:0,
        
        // --------- DATA FOR FREQUENCY CHART -------- //
        
        Fseries: [{
            
            seriesIndex: 0,
            name: 'Frequência',
            measure: "Time",
            data: []
            
        }],
        
        
        Foptions: {
            
            chart: {
                    id: 'Fchart',
            },
            
            xaxis: {
                
                type: "datetime",
                markers: {
                    size: 5
                },
                discrete: [{
                  seriesIndex: 0,
                  // For dataPointIndex: 0 there is no discrete marker at all!
                  dataPointIndex: 1,
                  
                  // Also docs says its "fill" and "stroke", but "fillColor" and "strokeColor" are working
                  fillColor: "#0A0",
                  strokeColor: "#FFF",
                  size: 7
                }],
                title: {
                    text: 'Tempo',
                    offsetY: 15
                },
                labels: {
                    formatter: function(val) {
                        console.log(val)
                        return new Date(val).toLocaleString('en-GB').slice(11,20)
                      }
                }
                
            }, 
            
            yaxis: {
              title: {
                text: 'Frequência [Hz]'
              }
            },
            
            colors: ['#00E396'],
            
            grid: {
                show: true,
                borderColor: '#90A4AE',
                strokeDashArray: 0,
                position: 'back',
                xaxis: {
                    lines: {
                        show: true
                    }
                },   
                yaxis: {
                    lines: {
                        show: true
                    }
                },  
                row: {
                    colors: undefined,
                    opacity: 0.5
                },  
                column: {
                    colors: undefined,
                    opacity: 0.5
                },  
                padding: {
                    top: 0,
                    right: 0,
                    bottom: 0,
                    left: 10
                },  
            }
              
        },
        
        // --------- /DATA FOR FREQUENCY CHART -------- //
        
        // --------- DATA FOR INTENSITY CHART -------- //
        
        Iseries: [{
            
            seriesIndex: 0,
            name: 'Corrente',
            measure: "Time",
            data: []
            
        }],
        
        
        Ioptions: {
            
            chart: {
                    id: 'Ichart',
            },
            
            xaxis: {
                
                type: "datetime",
                markers: {
                    size: 1
                },
                title: {
                    text: 'Tempo',
                    offsetY: 15
                },
                labels: {
                    formatter: function(val) {
                        console.log(val)
                        return new Date(val).toLocaleString('en-GB').slice(11,20)
                      }
                }
                
            }, 
            
            yaxis: {
              title: {
                text: 'Corrente [A]'
              }
            },
            
            colors: ['#F9CE1D'],
            
            grid: {
                show: true,
                borderColor: '#90A4AE',
                strokeDashArray: 0,
                position: 'back',
                xaxis: {
                    lines: {
                        show: true
                    }
                },   
                yaxis: {
                    lines: {
                        show: true
                    }
                },  
                row: {
                    colors: undefined,
                    opacity: 0.5
                },  
                column: {
                    colors: undefined,
                    opacity: 0.5
                },  
                padding: {
                    top: 0,
                    right: 0,
                    bottom: 0,
                    left: 10
                },  
            }
              
        },
        
        // --------- /DATA FOR INTENSITY CHART -------- //
        
        // --------- DATA FOR VOLTAGE CHART -------- //
        
        Vseries: [{
            
            seriesIndex: 0,
            name: 'Tensão',
            measure: "Time",
            //data: [[1991, 30],[1992,40], [1993, 45], [1994, 50], [1995, 50], [1996, 60], [1997, 70], [1998, 91]]
            data: []
            
        }],
        
        
        Voptions: {
            
            chart: {
                    id: 'Vchart',
            },
            
            xaxis: {
                
                type: "datetime",
                markers: {
                    size: 1
                },
                title: {
                    text: 'Tempo',
                    offsetY: 15
                },
                labels: {
                    formatter: function(val) {
                        console.log(val)
                        return new Date(val).toLocaleString('en-GB').slice(11,20)
                      }
                }
                
            }, 
            
            yaxis: {
              title: {
                text: 'Tensão Debitada [V]'
              }
            },
            
            colors: ['#FA4443'],
            
            grid: {
                show: true,
                borderColor: '#90A4AE',
                strokeDashArray: 0,
                position: 'back',
                xaxis: {
                    lines: {
                        show: true
                    }
                },   
                yaxis: {
                    lines: {
                        show: true
                    }
                },  
                row: {
                    colors: undefined,
                    opacity: 0.5
                },  
                column: {
                    colors: undefined,
                    opacity: 0.5
                },  
                padding: {
                    top: 0,
                    right: 0,
                    bottom: 0,
                    left: 10
                },  
            }
              
        }
        
        // --------- /DATA FOR VOLTAGE CHART -------- //
        
        
    }, // --- End of data --- //
    
    // computed: dynamic data, used as {{ cName }} - cached
    computed: {
    }, // --- End of computed --- //
    
    // methods:
    methods: {
        /** Invoked when user changes tab - saves current tab - @see mounted
         * @param {number} i Selected tab index number
         */
        changeTab: function (i) {
            // Save to browser's session storage
            sessionStorage.currentTab = i;
        },
        /** Switches between started and stopped (not in production) state of MIP83.
         *
         * */
        press_1(evt) {
            evt.preventDefault();
            uibuilder.send({
                'topic': 'freq',
                'payload':this.message
            })
        },
        /** Switches between started and stopped (not in production) state of MIP83.
         *
         * */
        press_2(evt) {
            evt.preventDefault(),
            this.message = ""
            this.value = 0
        },
        
        press_3(evt) {
            evt.preventDefault();
            uibuilder.send({
                'topic': 'freq',
                'payload':20
            })
        },
        
        press_4(evt) {
            evt.preventDefault();
            uibuilder.send({
                'topic': 'freq',
                'payload':0
            })
        },
        
        press_Man(evt) {
            evt.preventDefault();
            uibuilder.send({
                'topic': 'freq',
                'payload': 0
            })
        },
        
        press_Rem(evt) {
            evt.preventDefault();
            uibuilder.send({
                'topic': 'valRem',
                'payload': 1
            })
        },
        
        getNow: function() {
            const today = new Date();
            const date = today.getFullYear()+'-'+(today.getMonth()+1)+'-'+today.getDate();
            const time = today.getHours() + ":" + today.getMinutes() + ":" + today.getSeconds();
            const dateTime = date +' '+ time;
            
            return time
        },
          
          updateChartRT(evt) {
            this.series = [
                {
                seriesIndex: 0,
                name: 'NewSeries',
                measure: "Time",
                data: this.seriesFreq
              }
            ]
          },
          
          updateChartF(evt) {
            this.Fseries = [
                {
                seriesIndex: 0,
                name: 'Frequência [Hz]',
                data: this.seriesFreq
              }
            ]
          },
          
          updateChartI(evt) {
            this.Iseries = [
                {
                seriesIndex: 0,
                name: 'Corrente [A]',
                data: this.seriesInt
              }
            ]
          },
          
          updateChartT(evt) {
            this.Vseries = [
                {
                seriesIndex: 0,
                name: 'Tensão Debitada [V]',
                data: this.seriesVolt
              }
            ]
          }
        
    }, // --- End of methods --- //

    // Available hooks: init,mounted,updated,destroyed
    mounted: function () {
        uibuilder.start()
        var vueApp = this
        var timestamp

        //#region ---- Trace Received Messages ---- //
        // If msg changes - msg is updated when a standard msg is received from Node-RED over Socket.IO
        // newVal relates to the attribute being listened to.
        
        uibuilder.onChange('msg', function (newVal) {
            
            vueApp.msgRecvd = newVal
            
            if (newVal.topic == "topic1") {
                vueApp.var1 = newVal.payload
            }
            if (newVal.topic == "freq") {
                vueApp.var1 = newVal.payload
                vueApp.value = newVal.payload
            }
            if (newVal.topic == "freqenv") {
                vueApp.var11 = newVal.payload
            }
            if (newVal.topic === "intensidadeenv") {
                vueApp.var2 = newVal.payload
            }    
            if (newVal.topic == "tensaoenv") {
                vueApp.var3 = newVal.payload
            }
            
            if (newVal.topic == "topic2") {
                vueApp.var2 = newVal.payload
            }
            if (newVal.topic == "credentials") {
                vueApp.username = newVal.payload.inputUsername
                vueApp.password = newVal.payload.inputPassword
            }
            if (newVal.topic == "valRem") {
                vueApp.valorRemoto = newVal.payload
            }
            if (newVal.topic == "valMan") {
                vueApp.valorManual = newVal.payload
            }
            if (newVal.topic == "dbtime") {
                vueApp.time = newVal.payload
            }
            
            
            if (newVal.topic == "dbfreq") {
                
                console.log('Series')
                console.log(vueApp.series)
                console.log('Payload')
                console.log(newVal.payload)
                
                vueApp.seriesFreq = newVal.payload
                
                console.log('Series')
                console.log(vueApp.seriesFreq)
                
                /*
                vueApp.$refs.chart.updateSeries([{
                    data: vueApp.series
                }])     
                */
                
                vueApp.updateChart();
                
                //this.$refs.chart.updateSeries(this.series)
                
                console.log('Series')            
                console.log(vueApp.seriesFreq)
                
                
                //vueApp.$refs.chart.render()
                //ueApp.chart.refresh()
                
                console.log('Render passado')    
                
            }
            
            /*
            if (newVal.topic == "realtimechart") {
                
                
                vueApp.timestamp = vueApp.getNow(),
                
                //vueApp.timestamp = newVal.payload.now
                vueApp.rseries = vueApp.payload.rand
                 
                
                vueApp.$refs.chart.updateSeries([{
                      rdata: vueApp.payload.rseries
                    }])
                    
                vueApp.$refs.chart.updateOptions([{
                      xaxis: vueApp.payload.now
                    }])
                
                vueApp.rchart.render()
                
            }
            */
            
            
            if (newVal.topic == "dbfreqteste") {
                
                console.clear()
                
                vueApp.rand = newVal.payload
                
                
                if (vueApp.testearray.length > 9) {
                    
                    vueApp.testearray.push(vueApp.rand)
                    vueApp.testearray.shift()
                    
                } else {
                    
                    vueApp.testearray.push(vueApp.rand)
                    
                }
                
                vueApp.updateChartF();
                
                vueApp.updateChartI();
                
                vueApp.updateChartT();
                
            }
            
            if (newVal.topic == "dbFreq") {
                
                console.clear()
                
                vueApp.lastFreq = newVal.payload
                
                
                if (vueApp.seriesFreq.length > 9) {
                    
                    vueApp.seriesFreq.push(vueApp.lastFreq)
                    vueApp.seriesFreq.shift()
                    
                } else {
                    
                    vueApp.seriesFreq.push(vueApp.lastFreq)
                    
                }
                
                vueApp.updateChartF();
                
            }
            
            if (newVal.topic == "dbInt") {
                
                console.clear()
                
                vueApp.lastInt = newVal.payload
                
                
                if (vueApp.seriesFreq.length > 9) {
                    
                    vueApp.seriesInt.push(vueApp.lastInt)
                    vueApp.seriesInt.shift()
                    
                } else {
                    
                    vueApp.seriesInt.push(vueApp.lastInt)
                    
                }
                
                vueApp.updateChartI();
                
            }
            
            if (newVal.topic == "dbVolt") {
                
                console.clear()
                
                vueApp.lastVolt = newVal.payload
                
                
                if (vueApp.seriesVolt.length > 9) {
                    
                    vueApp.seriesVolt.push(vueApp.lastVolt)
                    vueApp.seriesVolt.shift()
                    
                } else {
                    
                    vueApp.seriesVolt.push(vueApp.lastVolt)
                    
                }
                
                vueApp.updateChartT;
                
            }
            
        }) // -- uibuilderonchange --//
            
            
    } // --- End of mounted hook --- //
      

}) // --- End of app1 --- //

// EOF
