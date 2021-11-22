for i in {0..1000..1}; do
  ./app ${i} >> test${i}.txt
done
