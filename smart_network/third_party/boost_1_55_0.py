#!/usr/bin/python
# -*- coding:utf-8 -*-

import os
import re
import tarfile
import threading
import urllib2
from subprocess import Popen, PIPE, STDOUT

rootpath = os.path.abspath(os.path.dirname(__file__))



class ArchiveReadyThread(threading.Thread):

  def __init__(self, num, params):
    threading.Thread.__init__(self)
    self.num = num
    self.basepath = os.path.join(rootpath, 'tmp')
    if not os.path.exists(self.basepath):
      os.makedirs(self.basepath)
    self.url     = params['url']
    self.success = params['success']
    self.failed  = params['failed']

  def download(self):
    num = self.num
    url = self.url
    basepath = self.basepath

    print '%d> download: %s' % (num, url)
    name = os.path.split(url)[1]
    path = os.path.join(basepath, name)
    response = urllib2.urlopen(url)
    data = response.read()
    tgz = open(path, "wb")
    tgz.write(data)
    tgz.close()
    return path

  def extract(self, filepath):
    num = self.num

    print '%d> extract: %s' % (num, filepath)
    tar = tarfile.open(filepath, 'r:gz')
    tar.extractall(self.basepath)
    tar.close()

    p = re.compile(r'(/|\\)(?P<name>[^/\\]+)(.tgz|.tar.gz)$')
    m = p.search(filepath)
    archpath = os.path.join(os.path.split(filepath)[0], m.group('name'))
    return archpath
	

  def run(self):
    filepath = self.download()
    archpath = self.extract(os.path.join(filepath))
    self.success(self.num, archpath)



def set_environments(num, envs):
  for key, val in envs.items():
    print '%d> %s=%s' % (num, key, val)
    os.environ[key] = str(val)

def failed(num, path):
  print  ('%d> failed') % num



boost = {
  'url': 'http://jaist.dl.sourceforge.net/project/boost/boost/1.55.0/boost_1_55_0.tar.gz',
  'success': (lambda num, path: None),
  'failed':  (lambda num, path: failed(num, path))
}

bzip2 = {
  'url': 'http://www.bzip.org/1.0.6/bzip2-1.0.6.tar.gz',
  'success': (lambda num, path: set_environments(num, {'BZIP2_SOURCE': path})),
  'failed':  (lambda num, path: failed(num, path))
}

icu4c = {
  'url': 'http://download.icu-project.org/files/icu4c/51.2/icu4c-51_2-src.tgz',
  'success': (lambda num, path: set_environments(num, {'ICU_PATH': os.path.abspath(os.path.join(path, '../icu'))})),
  'failed':  (lambda num, path: failed(num, path))
}

python27 = {
  'url': 'http://python.org/ftp/python/2.7.6/Python-2.7.6.tgz',
  'success': (lambda num, path: set_environments(num, {'PYTHON_ROOT': path, 'PYTHON_VERSION': '2.7'})),
  'failed':  (lambda num, path: failed(num, path))
}

zlib = {
  'url': 'http://zlib.net/zlib-1.2.8.tar.gz',
  'success': (lambda num, path: set_environments(num, {'ZLIB_SOURCE': path})),
  'failed':  (lambda num, path: failed(num, path))
}

threads = [
  ArchiveReadyThread(1, boost),
  ArchiveReadyThread(2, bzip2),
  ArchiveReadyThread(3, icu4c),
  ArchiveReadyThread(4, python27),
  ArchiveReadyThread(5, zlib)
]

for thread in threads:
  thread.start()
for thread in threads:
  thread.join()



is_win = (os.name == 'nt')

if is_win:
  """
  def run_batch(bat):
    import tempfile
    fd = tempfile.mkstemp(prefix='__', suffix='.bat', text=True)
    os.write(fd[0], bat)
    os.close(fd[0])

    call(fd[1], shell=True)
    os.remove(fd[1])
  """
  os.chdir(rootpath)
  pipe = Popen(os.path.join(rootpath, 'boost_1_55_0.bat'), stdout=PIPE, stderr=PIPE, shell=False)
  out,err = pipe.communicate()
  with open(os.path.join(rootpath, 'stdout.log'), 'w') as f:
    f.write(out)
  with open(os.path.join(rootpath, 'stderr.log'), 'w') as f:
    f.write(err)
