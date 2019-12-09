# DECLARANDO FUNCIONES UTILES PARA PODER SEPARAR LA DATA:

# SEPARAR LA SEÑAL POR ESTIMULO
def est_ind(estimulos):
    ind=[]
    for i in range(22):
        indice_inicial=estimulos[estimulos[0]==i+2].iloc[0].name #EXTRAE EL INDICE
        indice_final_anterior=indice_inicial-1
        ind.append(indice_final_anterior)
    ind.append(len(estimulos)-1)
    return ind
## Extrae los indices en base a la data cruda de estimulos
## Resultado: Vector de indices de comienzo de cada estímulo

def signal_stimulus(signal,indices):
    signal_by_est=[]
    for i in range(len(indices)):
        if (i==0):
            signal_by_est.append(signal[0:indices[i]])
        else:
            signal_by_est.append(signal[indices[i-1]+1:indices[i]])
    return signal_by_est
## Separa la señal data emg por indices en estimulos
## Resultado: Lista de señal de un electrodo por estímulo, dimensión 12 (Número de estimulos) 

def signaltotal_by_est(signal,estimulo):
    #Extraemos los indices de los estimulos
    indices=est_ind(estimulo)
    signaltotal=[]
    for i in range(len(signal)):
        signal_by_est=signal_stimulus(signal[i],indices)
        signaltotal.append(signal_by_est)
    return signaltotal
## Separa la señal por estimulos
## Resultado: Lista de electrodos, con una serie temporal por estimulo, dimensión 12 (#Est) x 16 (#Electrodos)

# SEPARAR ESTIMULO POR REPETICION
def rep_by_est(estimulo,repetition):
    rep_est=[]
    indices=est_ind(estimulo)
    for i in range(len(indices)):
        if (i==0):
            rep_est.append(repetition[0:indices[i]])
        else:
            rep_est.append(repetition[indices[i-1]+1:indices[i]])
    return rep_est
## Separa el vector repeticion por estimulos
## Resultado: Vector de repeticion por cada estimulo, dimensión 12 (#Est)

def ext_ind_rep(repetitions,num_rep):
    ind_rep_total=[]
    ind_rep_start=[]
    ind_rep_end=[]
    for i in range(num_rep):
        start=repetitions[repetitions[0]==i+1].iloc[0].name
        end=repetitions[repetitions[0]==i+1].iloc[-1].name
        ind_rep_start.append(start)
        ind_rep_end.append(end)
    ind_rep_total.append(ind_rep_start)
    ind_rep_total.append(ind_rep_end)
    return ind_rep_total
## Extrae los indices por repetición
## Resultado: Indices de start y final de cada señal de estimulo, dimensión 2x6 (#Rep)

def signal_separate(signal,repetitions,num_rep):
    signal_sep=[]
    indices=ext_ind_rep(repetitions,num_rep)
    for i in range(len(indices[0])):
        signal_by_rep=signal.loc[indices[0][i]:indices[1][i]]
        signal_sep.append(signal_by_rep)
    return signal_sep
## Separa una señal de un estímulo y de un electródo en series temporales de una repetición
## Resultados: Lista de serie temporal de cada repetición
