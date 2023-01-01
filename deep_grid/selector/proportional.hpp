#ifndef QD_PROPORTIONATE
#define QD_PROPORTIONATE

namespace sferes {
  namespace qd {
    namespace selector {      

      // Parent class for all fitness proportionate 
      template <typename Phen, typename Cell, typename Params> struct Fitness_prop 
      {
      public:	
	
	// Return total and relative fitnesses   // <--
	template<typename cell_t> std::tuple<float, std::vector<float>> relative_fitness(cell_t cell) const
	{
	  int size = cell->size();
      // Why init it to "1"?! (Probably all fitness values in the experiments are negative, but still...)
	  float least_fitness = 1;  //worst fitness value of the cell
	  std::vector<float> fitness(size); //relative fitness of each element of the cell
	  float total_fitness = 0; //sum of all relative fitnesses
	  for(size_t i=0; i<size; i++)
	    {
	      fitness[i] = cell->operator[](i)->fit().value();
	      least_fitness = std::min(least_fitness, fitness[i]);
	    }
	  for(size_t i=0; i<size; i++)
	    {
	      fitness[i] -= least_fitness;
	      total_fitness += fitness[i];
	    }
	  return {total_fitness, fitness};
	}

	// Return best indiv of a cell
	template<typename cell_t> int best_indiv(cell_t cell) const
	{
	  int size = cell->size();
	  float best_fitness = 0; //best fitness value of the cell
	  size_t indiv_num = 0;
	  for(size_t i=0; i<size; i++)
	  {
	    float fitness = cell->operator[](i)->fit().value();
	    if (fitness > best_fitness)
	    {
	      best_fitness = fitness;
	      indiv_num = i;
	    }
	  }
	  return indiv_num;
	}

	// Fitness proportionate    // (called with results from relative_fitness() above)
	int select_prop(int size, std::vector<float> relative_fit, float total_fit) const
	{
          if (size == 1)
	    return 0;
	  
	  if (total_fit == 0.0)
	    return misc::rand(size);
	  
	  int indiv_num = 0;
	  float r = misc::rand(total_fit);
	  float value = relative_fit[0];
	  while (value < r && indiv_num < (size - 1))
	    {
	      indiv_num++;
	      value += relative_fit[indiv_num];
	    }
	  return indiv_num;

      // Code-Reading:
      // (Assuming fitnesses are negative, see std::min() comment far above.)
      //
      // This returns a random choice weighted by how much better its fitness
      // is compared to the worst fitness within the same cell.
      //
      // Consequences:
      // - If the cell members have uniformly distributed fitness:
      //     It returns with 75% probability a member of the better half.
      // - If there exists a single, really really really bad individuum:
      //     It gives equal probability to all others. (Ignoring their relative fitness.)
      //     (If mutations have a small chance to be really bad, this may happen in practice.)
      //     (Would be really bad for the algo IMO.)
      // - If there exists a single, really really really good individuum:
      //     It returns it, ignoring all other fitnesses.
      //     (Elitism; Should be fine, considering that the algo randomly replaces a individuum.)
      //
      // Comment: Probably fitness should be rank-normalized first?! And then
      // reweighted somehow, similar to the cannonical ES...? Do a softmax choice?
      // Not sure exactly how. But rank-normalization would certainly be preferred.
      // The algo as-is drops the invariance to most fitness transforms.
	}
	

	// Fitness exponentially proportionate   // (never called)
	int select_exp(int size, std::vector<float> relative_fit, float total_fit) const
	{
          if (size == 1)
	    return 0;
	  
	  if (total_fit == 0.0)
	    return misc::rand(size);
	  
	  int indiv_num = 0;
	  float r = misc::rand(std::exp(total_fit));
	  float value = std::exp(relative_fit[0]);
	  while (value < r && indiv_num < (size - 1))
	    {
	      indiv_num++;
	      value *= std::exp(relative_fit[indiv_num]);
	    }
	  return indiv_num;
	}

      };
    } // namespace selector
  } // namespace qd
} // namespace sferes

#define DEEP_PROPORTIONATE(Class) \
 template <typename Phen, typename Cell, typename Params> \
	 class Class : public Fitness_prop<Phen, Cell, Params>

#endif
	
