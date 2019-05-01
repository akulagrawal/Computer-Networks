#!/usr/bin/env python
# coding: utf-8

# In[1]:


from pandas import read_csv
import numpy as np
import matplotlib.pyplot as plt
from pylab import rcParams


# In[2]:


get_ipython().run_line_magic('matplotlib', 'inline')


# In[3]:


rcParams['figure.figsize'] = 20, 10


# In[4]:


versions = ['NewReno', 'Hybla', 'Westwood', 'Scalable', 'Vegas']


# In[10]:


for version in versions:
    data = read_csv('./Output/Tcp'+version+'.cwnd', delimiter='\s', header=None, engine='python')
    X = data[0]
    Y = data[2]
    plt.plot(X, Y, label=version)
plt.legend()
plt.xlabel('Time (in seconds)')
plt.ylabel('CWND size')
plt.title('Comparision of Congestion Window size for various TCP versions')
plt.savefig('./Output/CWND Comparision.png')
plt.show()


# In[8]:


for version in versions:
    data = read_csv('./Output/Tcp'+version+'-packet-byte-count-0.txt', delimiter='\s', header=None, engine='python')
    X = data[0]
    Y = np.cumsum(data[1])
    plt.plot(X, Y, label=version)
plt.xlabel('Time (in seconds)')
plt.ylabel('Cumulative bytes transferred')
plt.title('Cumulative bytes transferred vs Time for various TCP versions')
plt.legend()
plt.savefig('./Output/Bytes Transferred.png')
plt.show()


# In[9]:


for version in versions:
    data = read_csv('./Output/Tcp'+version+'.drop', delimiter='\s', header=None, engine='python')
    X = data[0]
    Y = data[1]
    plt.plot(X, Y, label=version)

plt.xlabel('Time (in seconds)')
plt.ylabel('Cumulative packets dropped')
plt.title('Cumulative packets dropped vs Time for various TCP versions')
plt.legend()
plt.savefig('./Output/Packets dropped.png')
plt.show()


# In[ ]:




