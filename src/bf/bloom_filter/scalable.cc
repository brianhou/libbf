#include <bf/bloom_filter/scalable.h>

namespace bf {

scalable_bloom_filter::scalable_bloom_filter(size_t k, size_t cells, size_t seed)
  : k_(k), cells_(cells), seed_(seed)
{
  grow();
}

// TODO: Add a constructor that takes in error probability?

void scalable_bloom_filter::add(object const& o)
{
  static size_t added = 0, level = 0;
  added++;
  // TODO: Add a better check for full-ness. Currently counts elements added.
  if (added == std::pow(2, level))
  {
    added = 0;
    level++;
    grow();
  }
  levels_.back().add(o);
}

size_t scalable_bloom_filter::lookup(object const& o) const
{
  for (size_t i = 0; i < levels_.size(); ++i)
    if (levels_[i].lookup(o))
      return 1;
  return 0;
}

void scalable_bloom_filter::clear()
{
  levels_.clear();
  grow();
}

void scalable_bloom_filter::grow()
{
  auto l = levels_.size();

  // Double the cells with each new filter
  static size_t const min_size = 128;
  auto cells = min_size * std::pow(2, l);

  size_t seed = seed_;
  std::minstd_rand0 prng(seed);
  for (size_t i = 0; i < l; ++i)
    seed = prng();

  levels_.emplace_back(make_hasher(k_, seed), cells);
}

} // namespace bf
