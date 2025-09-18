#include <vector>


// --------------------------------- CLASE NODO --------------------------------- 
class Node {
    private:
        int id;       //un id para identificar al nodo                  
        double amplitude; //La amplitud que este va a tener
        double previous_amplitude; //La amplitud anterior que este va a tener
        std::vector<int> neighbors; //Los vecinos del nodos

    public:

        //Constructor del nodo
        Node(int node_id, double initial_amplitude = 0.0);
        //Getters
        int getId() const;
        double getAmplitude() const;
        double getPreviousAmplitude() const;
        const std::vector<int>& getNeighbors() const;
        int getDegree() const;


        //Setter
        void setAmplitude(double new_amplitude);
        void setPreviousAmplitude(double prev_amp);

        //Con esto agregamos los nodos
        void addNeighbor(int neighbor_id);

        void removeNeighbor(int neighbor_id);

        bool isNeighbor(int node_id) const;

        
    };


    